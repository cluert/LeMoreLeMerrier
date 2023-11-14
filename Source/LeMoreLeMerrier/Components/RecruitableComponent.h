// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "LeMoreLeMerrier/Subsystems/EconomySubsystem.h"
#include "RecruitableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecruited);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSaleMade);

UCLASS()
class LEMORELEMERRIER_API URecruitableComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	void BeginPlay();

	UEconomySubsystem* EconomySubsystem;

	UPROPERTY(BlueprintReadOnly)
	bool bRecruited = false;

	UPROPERTY(BlueprintReadOnly)
	int SalesMade = 0;

	UPROPERTY(BlueprintReadOnly)
	int TotalMoneyRaised = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RecruitmentCost = 100;

	UFUNCTION(BlueprintCallable)
	void Recruit();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRecruit();

	UFUNCTION(BlueprintCallable)
	void MakeOneSale();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMakeOneSale();

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	void GetSaleAmount(int& Amount);

	UPROPERTY(BlueprintAssignable)
	FRecruited OnRecruited;

	UPROPERTY(BlueprintAssignable)
	FRecruited OnSaleMade;
};
