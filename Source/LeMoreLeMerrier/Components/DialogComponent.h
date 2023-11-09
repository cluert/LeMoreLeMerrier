// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LeMoreLeMerrier/Dialog/MLMReader.h"
#include "DialogComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LEMORELEMERRIER_API UDialogComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDialogComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnFinishedReading();
	
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

	UFUNCTION(BlueprintPure)
	bool IsTreeValid()
	{
		return DialogTree.Message.IsEmpty();
	}

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, bool> SavedVariables;

	UFUNCTION(BlueprintCallable)
	bool GetVariable(const FString& VarName)
	{
		if (SavedVariables.Contains(VarName))
		{
			return SavedVariables[VarName];
		}
		return false;
	}

	UFUNCTION(BlueprintCallable)
	void LoadVariables(const TMap<FString,bool> NewValues)
	{
		SavedVariables = NewValues;
	}

	UFUNCTION(BlueprintCallable)
	void ClearVariables()
	{
		SavedVariables.Empty();
	}

	void SetValuesFromNode(FDialogNode Node)
	{
		SetValues(Node.Variables);
	}

	void SetValuesFromEdge(FDialogEdge Edge)
	{
		SetValues(Edge.Variables);
	}

	void SetValues(const TArray<FString>& Variables)
	{
		for (int32 i = 0; i < Variables.Num(); ++i)
		{
			FString VarName = Variables[i];
			if (!VarName.IsEmpty())
			{
				if (VarName.StartsWith("!"))
				{
					SavedVariables.Add(VarName.Mid(1), false);
				} else
				{
					SavedVariables.Add(VarName, true);
				}
			}
		}
	}
	
	UPROPERTY(BlueprintReadWrite)
	FString StartLabel;

	UPROPERTY(BlueprintReadWrite)
	FString DoneLabel;

	UFUNCTION(BlueprintCallable)
	FString GetErrorMessage()
	{
		if (DialogTree.Message.IsEmpty())
		{
			return TEXT("No error.");
		}
		else
		{
			return DialogTree.Message;
		}
	}
	
private:
	FDialogTree DialogTree;
	FDialogLine CurrentDialogLine;
	// Although named "current", these actually represent the NEXT Line that will be read
	// So we SAVE the actual current state in the reader, and use these values to indicate
	// the NEXT state
	int32 CurrentNode;
	int32 CurrentEdge;
	int32 CurrentLine;
	bool bNeedShowChoice;
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
