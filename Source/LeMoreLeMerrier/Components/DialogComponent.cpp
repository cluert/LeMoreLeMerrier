// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogComponent.h"


// Sets default values for this component's properties
UDialogComponent::UDialogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentNode = -1;
	CurrentEdge = -1;
	CurrentLine = -1;
	bAtChoice = false;
	bLastAtChoice = false;
	bNeedShowChoice = false;
	bDone = false;
	bLastDone = false;

	StartLabel = TEXT("Begin");
	DoneLabel = TEXT("Done");
}


// Called when the game starts
void UDialogComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDialogComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UDialogComponent::LoadScript(const FString& FileName)
{
	DialogTree = UMLMReader::GenerateDialogTree(FileName);
	if (DialogTree.Message.IsEmpty())
	{
		CurrentNode = GetNodeIndex(StartLabel);
		if (CurrentNode > -1)
		{
			CurrentEdge = -1;
			CurrentLine = -1;
			bAtChoice = false;
			bDone = false;
			bLastAtChoice = false;
			bLastDone = false;

			// Update indices without updating current-dialog-line (as they are currently invalid)
			// and then perform a normal update so we start by displaying the first line
			// (because our indices are 1 step ahead of the dialog being displayed)
			ProgressReader();
			Next();
			
			return true;
		}
	}


	// TODO notify of error -- but it's probably caused by not moving the scripts to the packaged game beforehand
	return false;
}

void UDialogComponent::Next()
{
	if (!IsTreeValid())
	{
		return;
	}
	
	if (bNeedShowChoice)
	{
		CurrentDialogLine.Speaker = TEXT("YOU");
		CurrentDialogLine.Line = GetFullChoiceText(CurrentEdge);
		bNeedShowChoice = false;
	}
	else
	{
		LoadCurrentDialogLine();
	}

	ProgressReader();
}

TArray<FString> UDialogComponent::ListChoices()
{
	if (!IsTreeValid())
	{
		TArray<FString> Empty;
		return Empty;
	}
	
	TArray<FString> Choices;
	int32 len = DialogTree.Nodes[CurrentNode].Edges.Num();
	for (int32 i = 0; i < len; ++i)
	{
		Choices.Add(DialogTree.Nodes[CurrentNode].Edges[i].ChoiceText);
	}
	return Choices;
}

void UDialogComponent::Choose(int32 ChosenIndex)
{
	if (!IsTreeValid())
	{
		return;
	}
	
	if (ChosenIndex < 0 || ChosenIndex >= DialogTree.Nodes[CurrentNode].Edges.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Choice was out of range (should be 0 to %i"), DialogTree.Nodes[CurrentNode].Edges.Num()-1)
		return;
	}

	CurrentEdge = ChosenIndex;
	CurrentLine = -1;
	bAtChoice = false;
	bNeedShowChoice = true;

	// UE_LOG(LogTemp, Warning, TEXT("After Choose, our indices are:  node=%i,   edge=%i,   line=%i"), CurrentNode, CurrentEdge, CurrentLine)
}

FString UDialogComponent::GetFullChoiceText(int32 ChoiceIndex)
{
	if (!IsTreeValid())
	{
		return TEXT("");
	}
	
	if (ChoiceIndex >= DialogTree.Nodes[CurrentNode].Edges.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Choice Index was out of range: asked for %i, current options %i"), ChoiceIndex, DialogTree.Nodes[CurrentNode].Edges.Num())
		return TEXT("");
	}

	FString FullText = DialogTree.Nodes[CurrentNode].Edges[ChoiceIndex].ChoiceTextExtended;
	if (FullText.IsEmpty())
	{
		return DialogTree.Nodes[CurrentNode].Edges[ChoiceIndex].ChoiceText;
	}
	return FullText;
}

bool UDialogComponent::ValidateTree()
{
	return false;
}

void UDialogComponent::ProgressReader()
{
	if (bDone || bAtChoice)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot increment reader! Reader is done or at a choice!  (done=%i   choice=%i"), bDone, bAtChoice)
		return;
	}

	int32 LineCount;
	
	// Try to get the next line of dialog
	if (InEdge())
	{
		LineCount = DialogTree.Nodes[CurrentNode].Edges[CurrentEdge].Lines.Num();
	}
	else
	{
		LineCount = DialogTree.Nodes[CurrentNode].Lines.Num();
	}

	// Check if we can just return the next line of dialog
	if (LineCount > CurrentLine+1)
	{
		++CurrentLine;
		return;
	}

	// We are out of lines to display for the current Node/Edge -- try to move through graph

	// (but first, save variables)
	if (InEdge())
	{
		SetValuesFromEdge(DialogTree.Nodes[CurrentNode].Edges[CurrentEdge]);
	}
	else
	{
		SetValuesFromNode(DialogTree.Nodes[CurrentNode]);
	}
	
	if (InEdge())
	{
		// Get the node the current edge connects to.  If there is none, we are done reading
		
		FString NextNodeId = DialogTree.Nodes[CurrentNode].Edges[CurrentEdge].NextNodeId;
		if (!NextNodeId.IsEmpty() && 0 != NextNodeId.Compare(DoneLabel, ESearchCase::IgnoreCase))
		{
			CurrentNode = GetNodeIndex(NextNodeId);
			if (CurrentNode > -1)
			{
				// Attempt to progress from the next node
				CurrentEdge = -1;
				CurrentLine = -1;
				return ProgressReader();
			}

			UE_LOG(LogTemp, Warning, TEXT("Mismatched GoTo Label!! NextNodeId=%s but no such node was found"), *NextNodeId)
		}

		// No next node, we are done
		bDone = true;
		return;
	}

	int32 EdgeCount = DialogTree.Nodes[CurrentNode].Edges.Num();
	if (EdgeCount == 0)
	{
		bDone = true;
	}
	else if (EdgeCount == 1)
	{
		CurrentEdge = 0;
		CurrentLine = -1;
		return ProgressReader();
	}
	else
	{
		bAtChoice = true;
	}
}


