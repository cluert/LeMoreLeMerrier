// Fill out your copyright notice in the Description page of Project Settings.


#include "LeMoreLeMerrier/Widgets/ButtonTutorial.h"

void UButtonTutorial::NativeConstruct()
{
	Super::NativeConstruct();

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
    BindToInputAction(Input);
}

void UButtonTutorial::BindToInputAction(UInputComponent* PlayerInputComponent)
{
    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    // You can bind to any of the trigger events here by changing the "ETriggerEvent" enum value
    Input->BindAction(InputAction, ETriggerEvent::Triggered, this, &UButtonTutorial::OnButtonPressed);
    OnBoundToInputAction();
}