// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogWidget.generated.h"

/**
 * 
 */
UCLASS()
class LEMORELEMERRIER_API UDialogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ExposeOnSpawn = true), Category="Dialog")
	class UDialogComponent* DialogReader;

};
