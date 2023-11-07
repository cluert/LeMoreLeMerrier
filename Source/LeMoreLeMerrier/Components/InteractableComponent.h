// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class UInteracterComponent*, Interacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopInteracting, class UInteracterComponent*, Interacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocus, class UInteracterComponent*, Interacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopFocusing, class UInteracterComponent*, Interacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformInteraction, class UInteracterComponent*, Interacter);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LEMORELEMERRIER_API UInteractableComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractableComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableActionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bAllowMultipleInteractors;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnInteract OnInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnStopInteracting OnStopInteracting;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnFocus OnFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnStopFocusing OnStopFocusing;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnPerformInteraction OnPerformInteraction;
	
	UFUNCTION()
	// Client function called when looking at interactable
	void Focus(class UInteracterComponent* Interacter);

	UFUNCTION()
	// Client function called when stopped looking at interactable
	void StopFocusing(class UInteracterComponent* Interacter);
	
	UFUNCTION()
	// Client function called when attempting interaction with interactable
	void Interact(class UInteracterComponent* Interacter);

	UFUNCTION()
	// Client function called when stopping interaction with interactable
	void StopInteracting(class UInteracterComponent* Interacter);

	UFUNCTION()
	void PerformInteraction(class UInteracterComponent* Interacter);

protected:
	UPROPERTY()
	TArray<class UInteracterComponent*> Interactors;
	
	bool CanInteract(class UInteracterComponent* Interacter) const;


public:
	void RefreshWidget();

	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetInteractPercent();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CancelInteractions();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
};
