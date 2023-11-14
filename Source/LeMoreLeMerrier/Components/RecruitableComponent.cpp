// Fill out your copyright notice in the Description page of Project Settings.


#include "RecruitableComponent.h"
#include "LeMoreLeMerrier/Widgets/RecruitableWidget.h"

void URecruitableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	EconomySubsystem = GameInstance->GetSubsystem<UEconomySubsystem>();

	URecruitableWidget* RecruitableWidget = (URecruitableWidget*)GetUserWidgetObject();
	RecruitableWidget->OwningComponent = this;
}

void URecruitableComponent::Recruit()
{
	bRecruited = true;

	OnRecruit();
	OnRecruited.Broadcast();
}

void URecruitableComponent::MakeOneSale()
{
	int SaleAmount;
	GetSaleAmount(SaleAmount);
	EconomySubsystem->AddMoney(SaleAmount);

	SalesMade++;
	TotalMoneyRaised += SaleAmount;

	OnMakeOneSale();
	OnSaleMade.Broadcast();
}
