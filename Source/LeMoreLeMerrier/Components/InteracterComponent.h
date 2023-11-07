// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteracterComponent.generated.h"

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData() {
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}
	
	UPROPERTY()
	class UInteractableComponent* ViewedInteractionComponent;

	UPROPERTY()
	float LastInteractionCheckTime;

	UPROPERTY()
	bool bInteractHeld;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LEMORELEMERRIER_API UInteracterComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FInteractionData InteractionData;
	
public:
	UInteracterComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance = 500.f;

	UFUNCTION(BlueprintCallable)
	bool IsInteracting() const;

	UFUNCTION(BlueprintCallable)
	float GetRemainingInteractionTime() const;
	
	UFUNCTION()
	void TraceForInteractable();

protected:
	FORCEINLINE class UInteractableComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void HandleFoundInteractable(class UInteractableComponent* Interactable);

	UFUNCTION()
	virtual void HandleLostInteractable();
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Intended to be called by input functions when the interact key is first pressed
	UFUNCTION(BlueprintCallable)
	void Interact();

	// Intended to be called by input functions when the interact key is released
	UFUNCTION(BlueprintCallable)
	void StopInteracting();

	UFUNCTION()
	void PerformInteraction();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(class UInteractableComponent* Interactable);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopInteracting();

private:
	FTimerHandle TimerHandle_Interact;
};
