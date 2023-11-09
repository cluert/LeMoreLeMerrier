// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EconomySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoneyChanged, int, NewMoney);

UCLASS()
class LEMORELEMERRIER_API UEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetMoney();
	UFUNCTION(BlueprintCallable, BlueprintPure)

	bool CanAfford(int Cost);
	UFUNCTION(BlueprintCallable)
	void SetMoney(int NewMoney, bool Clamp);
	UFUNCTION(BlueprintCallable)
	void AddMoney(int MoneyToAdd);
	UFUNCTION(BlueprintCallable, meta=(ReturnDisplayName="Success"))
	bool TrySpend(int Cost);

	UPROPERTY(BlueprintAssignable)
	FMoneyChanged OnMoneyChanged;

private:
	int Money = 0;
};
