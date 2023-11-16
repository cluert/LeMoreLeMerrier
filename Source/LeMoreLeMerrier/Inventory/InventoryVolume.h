// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "LeMoreLeMerrier/Components/ProductComponent.h"
#include "InventoryVolume.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);

USTRUCT(BlueprintType)
struct FInventoryItemCategory
{
	GENERATED_BODY()
public:
	// Array of ProductComponent instances
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UProductComponent*> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int Count;

	void AddItem(UProductComponent* Item)
	{
		Items.Add(Item);
		Count++;
	}

	void RemoveItem(UProductComponent* Item)
	{
		Items.Remove(Item);
		Count--;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bShowInHotbar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSoftObjectPtr< UTexture2D > Icon;
};

/*
Actor that tracks all of the products inside of the player's home, treating them as if
they are part of an inventory.
*/
UCLASS( Blueprintable, BlueprintType, ClassGroup=(Economy) )
class LEMORELEMERRIER_API AInventoryVolume : public AActor
{
	GENERATED_BODY()

public:
	AInventoryVolume();

	void BeginPlay() override;

	// Box collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent;

	// Dictionary of Uclasses mapped to corresponding FInventoryItemCategory structs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<UClass*, FInventoryItemCategory> Items;

	UFUNCTION()
	void OnOverlapBegin(
		class UPrimitiveComponent* OverlappedComponent,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		class UPrimitiveComponent* OverlappedComponent,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddProduct(UProductComponent* Product);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnAddProduct(UProductComponent* Product);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveProduct(UProductComponent* Product);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnRemoveProduct(UProductComponent* Product);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryChanged OnInventoryChanged;
};
