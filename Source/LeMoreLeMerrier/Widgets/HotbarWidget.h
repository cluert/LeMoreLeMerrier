// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LeMoreLeMerrier/Inventory/InventoryVolume.h"
#include "HotbarWidget.generated.h"

/**
 * 
 */
UCLASS()
class LEMORELEMERRIER_API UHotbarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AInventoryVolume* InventoryVolume;

	UFUNCTION(BlueprintCallable)
	void BindToInventoryVolume(AInventoryVolume* BindToInventory);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBoundToInventory(AInventoryVolume* Inventory);
};
