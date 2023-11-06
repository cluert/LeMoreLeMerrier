// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MLMReader.h"
#include "MLMPresenter.generated.h"

UCLASS(Blueprintable)
class LEMORELEMERRIER_API AMLMPresenter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMLMPresenter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FDialogLine GetCurrentLine()
	{
		return CurrentDialogLine;
	}

	UFUNCTION(BlueprintCallable)
	bool LoadScript(const FString& FileName);
	
	UFUNCTION(BlueprintCallable)
	void Next();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AtChoice() { return bAtChoice; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasMoreDialog() { return bDone; }
	
	UFUNCTION(BlueprintCallable)
	TArray<FString> ListChoices();

	UFUNCTION(BlueprintCallable)
	void Choose(int32 ChosenIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetFullChoiceText(int32 ChoiceIndex);

	UFUNCTION(BlueprintCallable)
	bool ValidateTree();

	UPROPERTY(BlueprintReadWrite)
	FString StartLabel;
	
private:
	FDialogTree DialogTree;
	FDialogLine CurrentDialogLine;
	// Although named "current", these actually represent the NEXT Line that will be read
	// So we SAVE the actual current state in the reader, and use these values to indicate
	// the NEXT state
	int32 CurrentNode;
	int32 CurrentEdge;
	int32 CurrentLine;
	bool bAtChoice;
	bool bDone;
	bool bLastAtChoice;
	bool bLastDone;

	void LoadCurrentDialogLine()
	{
		if (CurrentEdge > -1)
		{
			CurrentDialogLine = DialogTree.Nodes[CurrentNode].Edges[CurrentEdge].Lines[CurrentLine];
		}
		else
		{
			CurrentDialogLine = DialogTree.Nodes[CurrentNode].Lines[CurrentLine];
		}
	}
	void ProgressReader();

	bool InEdge() { return CurrentEdge > -1; }

	int32 GetNodeIndex(const FString& Label)
	{
		for (int32 i = 0; i < DialogTree.Nodes.Num(); ++i)
		{
			if (0 == DialogTree.Nodes[i].Id.Compare(Label, ESearchCase::IgnoreCase))
			{
				return i;
			}
		}

		return -1;
	}
};
