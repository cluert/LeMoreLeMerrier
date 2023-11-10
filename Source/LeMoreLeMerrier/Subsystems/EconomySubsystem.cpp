// Fill out your copyright notice in the Description page of Project Settings.


#include "EconomySubsystem.h"

void UEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Display, TEXT("Initializing economy subsystem"));
}

void UEconomySubsystem::Deinitialize()
{
	return;
}

int UEconomySubsystem::GetMoney()
{
	return Money;
}

bool UEconomySubsystem::CanAfford(int Cost)
{
	return GetMoney() >= Cost;
}

void UEconomySubsystem::SetMoney(int NewMoney, bool Clamp=true)
{
	Money = NewMoney;
	if (Clamp) {
		Money = FMath::Clamp(Money, 0, INT_MAX);
	}
	OnMoneyChanged.Broadcast(GetMoney());
	UE_LOG(LogTemp, Display, TEXT("Updating money to %d"), GetMoney())
}

void UEconomySubsystem::AddMoney(int MoneyToAdd)
{
	SetMoney(GetMoney() + MoneyToAdd);
}

bool UEconomySubsystem::TrySpend(int Cost)
{
	if (CanAfford(Cost)) {
		SetMoney(GetMoney() - Cost);
		return true;
	}
	else
	{
		return false;
	}
}
