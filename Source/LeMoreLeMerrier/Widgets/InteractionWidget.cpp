// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"
#include "LeMoreLeMerrier/Components/InteractableComponent.h"

void UInteractionWidget::UpdateInteractionWidget(UInteractableComponent* InteractableComponent)
{
	OwningComponent = InteractableComponent;
	OnUpdateInteractionWidget();
}
