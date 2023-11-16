// Fill out your copyright notice in the Description page of Project Settings.


#include "LeMoreLeMerrier/Inventory/InventoryVolume.h"

// Constructor

AInventoryVolume::AInventoryVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the box component
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetCollisionProfileName(TEXT("BuyVolume"));
	BoxComponent->SetGenerateOverlapEvents(true);
	bGenerateOverlapEventsDuringLevelStreaming = true;
	// Bind to BoxComponent's Begin and End Overlap events
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInventoryVolume::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AInventoryVolume::OnOverlapEnd);
}

void AInventoryVolume::BeginPlay()
{
	Super::BeginPlay();

	// Get a list of all overlapping actors
	TArray<AActor*> OverlapActors;
	BoxComponent->GetOverlappingActors(OverlapActors);
	for (AActor* OverlapActor : OverlapActors)
	{
		// Check if the overlapped actor has a UProductComponent
		UProductComponent* ProductComponent = Cast<UProductComponent>(OverlapActor->FindComponentByClass<UProductComponent>());
		if (ProductComponent)
		{
			AddProduct(ProductComponent);
		}
	}
}

void AInventoryVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapped actor has a UProductComponent
	UProductComponent* OtherProductComponent = Cast<UProductComponent>(OtherActor->FindComponentByClass<UProductComponent>());
	if (OtherProductComponent)
	{
		AddProduct(OtherProductComponent);
	}
}

void AInventoryVolume::AddProduct(UProductComponent* Product)
{
	FInventoryItemCategory* ItemCategory = Items.Find(Product->GetOwner()->GetClass());
	if (ItemCategory != nullptr)
	{
		ItemCategory->AddItem(Product);
		Product->AddedToInventory();
		OnAddProduct(Product);
		OnInventoryChanged.Broadcast();
	}
}

void AInventoryVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if the overlapped actor has a UProductComponent
	UProductComponent* OtherProductComponent = Cast<UProductComponent>(OtherActor->FindComponentByClass<UProductComponent>());
	if (OtherProductComponent)
	{
		RemoveProduct(OtherProductComponent);
	}
}

void AInventoryVolume::RemoveProduct(UProductComponent* Product)
{
	FInventoryItemCategory* ItemCategory = Items.Find(Product->GetOwner()->GetClass());
	if (ItemCategory != nullptr)
	{
		ItemCategory->RemoveItem(Product);
		Product->RemovedFromInventory();
		OnRemoveProduct(Product);
		OnInventoryChanged.Broadcast();
	}
}
