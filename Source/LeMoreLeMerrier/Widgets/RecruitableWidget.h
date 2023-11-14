// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecruitableWidget.generated.h"

/**
 * 
 */
UCLASS()
class LEMORELEMERRIER_API URecruitableWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn))
	class URecruitableComponent* OwningComponent;
};
