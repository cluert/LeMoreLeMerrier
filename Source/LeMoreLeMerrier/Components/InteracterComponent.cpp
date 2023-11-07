// Fill out your copyright notice in the Description page of Project Settings.


#include "InteracterComponent.h"
#include "InteractableComponent.h"

// Sets default values for this component's properties
UInteracterComponent::UInteracterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UInteracterComponent::IsInteracting() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Interact);
}

float UInteracterComponent::GetRemainingInteractionTime() const
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void UInteracterComponent::TraceForInteractable()
{
	AActor* Owner = GetOwner();
	APawn* Pawn = Cast<APawn>(Owner);
	if (Pawn == nullptr) {
		return;
	}
	APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
	if (Controller == nullptr) {
		return;
	}

	if (!Pawn->IsLocallyControlled()) {
		return;
	}

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyeLocation;
	FRotator EyeRotation;
	Controller->GetPlayerViewPoint(EyeLocation, EyeRotation);

	FVector TraceStart = EyeLocation;
	FVector TraceEnd = (EyeRotation.Vector() * InteractionCheckDistance) + TraceStart;
	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams)) {
		if (TraceHit.GetActor()) {
			if (UInteractableComponent* InteractionComponent = Cast<UInteractableComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractableComponent::StaticClass()))) {
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();

				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance) {
					HandleFoundInteractable(InteractionComponent);
				} else if (GetInteractable() && Distance > InteractionComponent->InteractionDistance) {
					HandleLostInteractable();
				}

				return;
			}
		}
	}

	HandleLostInteractable();
}

void UInteracterComponent::HandleFoundInteractable(UInteractableComponent* Interactable)
{
	StopInteracting();

	if (UInteractableComponent* PreviousInteractable = GetInteractable()) {
		PreviousInteractable->StopFocusing(this);
	}
	
	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->Focus(this);
}

void UInteracterComponent::HandleLostInteractable()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractableComponent* Interactable = GetInteractable()) {
		Interactable->StopFocusing(this);
		
		if (InteractionData.bInteractHeld) {
			StopInteracting();
		}
	}
	
	InteractionData.ViewedInteractionComponent = nullptr;
}

void UInteracterComponent::Interact()
{
	if (!GetOwner()->HasAuthority()) {
		ServerInteract(GetInteractable());
	}

	InteractionData.bInteractHeld = true;

	if (UInteractableComponent* Interactable = GetInteractable()) {
		Interactable->Interact(this);

		if (FMath::IsNearlyZero(Interactable->InteractionTime)) {
			PerformInteraction();
		} else {
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Interact, this, &UInteracterComponent::PerformInteraction, Interactable->InteractionTime, false);
		}
	}
}

void UInteracterComponent::StopInteracting()
{
	if (!GetOwner()->HasAuthority()) {
		ServerStopInteracting();
	}

	InteractionData.bInteractHeld = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractableComponent* Interactable = GetInteractable()) {
		Interactable->StopInteracting(this);
	}
}

void UInteracterComponent::PerformInteraction()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractableComponent* Interactable = GetInteractable()) {
		Interactable->PerformInteraction(this);
	}
}

void UInteracterComponent::ServerInteract_Implementation(UInteractableComponent* Interactable)
{
	InteractionData.ViewedInteractionComponent = Interactable;
	Interact();
}

bool UInteracterComponent::ServerInteract_Validate(UInteractableComponent* Interactable)
{
	if (Interactable) {
		if (AActor* A = Cast<AActor>(GetOwner())) {
			if (AActor* B = Cast<AActor>(Interactable->GetOwner())) {
				return (A->GetActorLocation() - B->GetActorLocation()).Size() <= Interactable->InteractionDistance + 100.f;
			}
		}
	}
	
	return false;
}

void UInteracterComponent::ServerStopInteracting_Implementation()
{
	StopInteracting();
}

bool UInteracterComponent::ServerStopInteracting_Validate()
{
	return true;
}

void UInteracterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteracterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceForInteractable();
}

