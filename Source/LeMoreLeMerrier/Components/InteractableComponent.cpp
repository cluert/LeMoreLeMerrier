// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"
#include "InteracterComponent.h"
#include "LeMoreLeMerrier/Widgets/InteractionWidget.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true; // if this is false, will not display widget
	SetComponentTickEnabled(false); // ... but THIS can be false?  whaaaatttt is the difference??


	InteractionTime = 0.f;
	InteractionDistance = 400.f;
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");
	bAllowMultipleInteractors = true;

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(600, 100);
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true);
}

void UInteractableComponent::Focus(UInteracterComponent* Interacter)
{
	if (!IsActive() || !GetOwner() || !Interacter) {
		return;
	}

	OnFocus.Broadcast(Interacter);

	SetHiddenInGame(false);

	// Create halo around the parent object if using Reuben Ward's post-process material
	// if (GetOwner()->GetNetMode() != NM_DedicatedServer) {
	// 	for (auto& VisualComp : GetOwner()->GetComponents(UStaticMeshComponent::StaticClass())) {
	// 		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp)) {
	// 			Prim->SetRenderCustomDepth(true);
	// 		}
	// 	}
	// }

	RefreshWidget();
}

void UInteractableComponent::StopFocusing(UInteracterComponent* Interacter)
{
	OnStopFocusing.Broadcast(Interacter);

	if (GetNetMode() != NM_DedicatedServer)
	{
		SetHiddenInGame(true);

		// for (auto& VisualComp : GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		// {
		// 	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		// 	{
		// 		Prim->SetRenderCustomDepth(false);
		// 	}
		// }	
	}
}

void UInteractableComponent::Interact(UInteracterComponent* Interacter)
{
	if (CanInteract(Interacter))
	{
		Interactors.AddUnique(Interacter);
		OnInteract.Broadcast(Interacter);
	}
}

void UInteractableComponent::StopInteracting(UInteracterComponent* Interacter)
{
	Interactors.RemoveSingle(Interacter);
	OnStopInteracting.Broadcast(Interacter);
}

void UInteractableComponent::PerformInteraction(UInteracterComponent* Interacter)
{
	if (CanInteract(Interacter))
	{
		OnPerformInteraction.Broadcast(Interacter);
		// UE_LOG(LogTemp, Warning, TEXT("Interaction Performed."))
	}
}

bool UInteractableComponent::CanInteract(UInteracterComponent* Interacter) const
{
	const bool bInteractCountOk = bAllowMultipleInteractors || Interactors.Num() < 1;
	return bInteractCountOk && IsActive() && GetOwner() != nullptr && Interacter != nullptr;
}

void UInteractableComponent::RefreshWidget()
{
	if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
	{
		InteractionWidget->UpdateInteractionWidget(this);
	}
}

float UInteractableComponent::GetInteractPercent()
{
	if (Interactors.IsValidIndex(0))
	{
		if (UInteracterComponent* Interacter = Interactors[0])
		{
			if (Interacter && Interacter->IsInteracting())
			{
				return 1.f - FMath::Abs(Interacter->GetRemainingInteractionTime() / InteractionTime);
			}
		}
	}
	return 0.f;
}

void UInteractableComponent::CancelInteractions()
{
	for (int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (UInteracterComponent* Interactor = Interactors[i])
		{
			StopFocusing(Interactor);
			StopInteracting(Interactor);
		}
	}

	Interactors.Empty();
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
