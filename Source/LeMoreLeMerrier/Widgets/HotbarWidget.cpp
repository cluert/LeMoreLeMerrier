// Fill out your copyright notice in the Description page of Project Settings.


#include "LeMoreLeMerrier/Widgets/HotbarWidget.h"

void UHotbarWidget::BindToInventoryVolume(AInventoryVolume* BindToInventory)
{
	InventoryVolume = BindToInventory;

	OnBoundToInventory(BindToInventory);
}
