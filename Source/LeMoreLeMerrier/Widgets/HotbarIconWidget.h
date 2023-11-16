// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LeMoreLeMerrier/Inventory/InventoryVolume.h"
#include "HotbarWidget.h"
#include "HotbarIconWidget.generated.h"

/**
 Widget that displays an icon for one item category in the hotbar.
 */
UCLASS()
class LEMORELEMERRIER_API UHotbarIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	UHotbarWidget* OwningHotbarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", Meta = (ExposeOnSpawn = true))
	FInventoryItemCategory ItemCategory;
};
