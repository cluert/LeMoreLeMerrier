// Fill out your copyright notice in the Description page of Project Settings.


#include "MLMPresenter.h"

// Sets default values
AMLMPresenter::AMLMPresenter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StartLabel = TEXT("Begin");
}

// Called when the game starts or when spawned
void AMLMPresenter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMLMPresenter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AMLMPresenter::LoadScript(const FString& FileName)
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

void AMLMPresenter::Next()
{
	LoadCurrentDialogLine();

	ProgressReader();
}

TArray<FString> AMLMPresenter::ListChoices()
{
	TArray<FString> Choices;
	int32 len = DialogTree.Nodes[CurrentNode].Edges.Num();
	for (int32 i = 0; i < len; ++i)
	{
		Choices.Add(DialogTree.Nodes[CurrentNode].Edges[i].ChoiceText);
	}
	return Choices;
}

void AMLMPresenter::Choose(int32 ChosenIndex)
{
	if (ChosenIndex < 0 || ChosenIndex >= DialogTree.Nodes[CurrentNode].Edges.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Choice was out of range (should be 0 to %i"), DialogTree.Nodes[CurrentNode].Edges.Num()-1)
		return;
	}

	CurrentEdge = ChosenIndex;
	CurrentLine = -1;
	bAtChoice = false;
	ProgressReader();

	UE_LOG(LogTemp, Warning, TEXT("After Choose, our indices are:  node=%i,   edge=%i,   line=%i"), CurrentNode, CurrentEdge, CurrentLine)
}

FString AMLMPresenter::GetFullChoiceText(int32 ChoiceIndex)
{
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

bool AMLMPresenter::ValidateTree()
{
	return false;
}

void AMLMPresenter::ProgressReader()
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

	if (InEdge())
	{
		// Get the node the current edge connects to.  If there is none, we are done reading
		
		FString NextNodeId = DialogTree.Nodes[CurrentNode].Edges[CurrentEdge].NextNodeId;
		if (!NextNodeId.IsEmpty())
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

