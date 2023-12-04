// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputTriggers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "ButtonTutorial.generated.h"

/**
 * 
 */
UCLASS()
class LEMORELEMERRIER_API UButtonTutorial : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn=true))
	UInputAction* InputAction;

	void NativeConstruct();
	
	UFUNCTION(BlueprintCallable)
	void BindToInputAction(UInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnButtonPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnBoundToInputAction();
};
