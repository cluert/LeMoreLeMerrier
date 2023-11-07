// Fill out your copyright notice in the Description page of Project Settings.


#include "MLMReader.h"

FDialogTree UMLMReader::GenerateDialogTree(const FString& FileName)
{
	FString file = FPaths::ProjectContentDir();
	file.Append("MLM/Core/Dialog/Scripts/");
	file.Append(FileName);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString FileContent;

	if (!FileManager.FileExists(*file))
	{
		UE_LOG(LogTemp, Warning, TEXT("File not found: %s"), *file)
		FDialogTree FailedTree;
		FailedTree.Message = TEXT("File not found");
		return FailedTree;
	}

	if (!FFileHelper::LoadFileToString(FileContent, *file, FFileHelper::EHashOptions::None))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadFileToString: Failed to load text from file %s"), *file)
		FDialogTree FailedTree;
		FailedTree.Message = TEXT("File found but failed to load to string");
		return FailedTree;
	}
	
	TArray<FString> Lines;
	FileContent.ParseIntoArrayLines(Lines, true);

	FDialogTree Tree;
	int32 NodeIndex = -1;
	int32 EdgeIndex = -1;
	bool InEdge = false;


	bool DoingChoices = false;
	bool ChoiceNeedsMatch = false;
	bool LastMatchWasChoice = false;
	for (int32 i = 0; i < Lines.Num(); ++i)
	{
		FString Row = FormatRow(Lines[i]);
		if (Row.IsEmpty())
		{
			continue;
		}
		
		ERowType Type = GetRowType(Row);

		if (Type == ERowType::LABEL)
		{
		    if (ChoiceNeedsMatch) {
                FString Message = FString::Printf(TEXT("CHOOSE/GOTO IS MISSING PAIR SOMEWHERE BEFORE line %i (%s)"), i, *Row);
                UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
                Tree.Message = Message;
                return Tree;
		    }
		
			InEdge = false;
			DoingChoices = false;
			ChoiceNeedsMatch = false;
			EdgeIndex = -1;
			++NodeIndex;
			
			FDialogNode NewNode;
			NewNode.Id = GetTextAfterCommand(Row, Type);
			Tree.Nodes.Add(NewNode);
			// UE_LOG(LogTemp, Warning, TEXT("Node='%s'"), *NewNode.Id)
		}
		else if (Type == ERowType::DIALOG || Type == ERowType::SAY)
		{
			FDialogLine Line;

			if (Type == DIALOG)
			{
				Line = ParseLine(Row);
			} else if (Type == SAY)
			{
				Line.Speaker = TEXT("SAY");
				Line.Line = GetTextAfterCommand(Row, Type);
			}
			
			if (InEdge)
			{
				Tree.Nodes[NodeIndex].Edges[EdgeIndex].Lines.Add(Line);
			}
			else
			{
				Tree.Nodes[NodeIndex].Lines.Add(Line);
			}

			// UE_LOG(LogTemp, Warning, TEXT("speaker=%s,verse=%s"), *Line.Speaker, *Line.Line);
		}
		else if (Type == GOTO)
		{
			InEdge = true;
			
			if (DoingChoices && ChoiceNeedsMatch)
			{
				if (LastMatchWasChoice == false)
				{
					FString Message = FString::Printf(TEXT("Mismatch of CHOOSE and GOTO at line %i (%s)"), i, *Row);
					UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
					Tree.Message = Message;
					return Tree;
				}
				
				Tree.Nodes[NodeIndex].Edges[EdgeIndex].NextNodeId = GetTextAfterCommand(Row, Type);
				ChoiceNeedsMatch = false;

				// UE_LOG(LogTemp, Warning, TEXT("Choice-Goto = %s"), *GetTextAfterCommand(Row, Type))
			}
			else
			{
				++EdgeIndex;

				FDialogEdge Edge;
				Edge.NextNodeId = GetTextAfterCommand(Row, Type);
				Tree.Nodes[NodeIndex].Edges.Add(Edge);

				if (DoingChoices)
				{
					ChoiceNeedsMatch = true;
					LastMatchWasChoice = false;
				}

				// UE_LOG(LogTemp, Warning, TEXT("Goto = %s"), *GetTextAfterCommand(Row, Type))
			}
		}
		else if (Type == CHOOSE) // i think this logic can be combined with goto
		{
			DoingChoices = true;
			InEdge = true;
			
			if (ChoiceNeedsMatch)
			{
				if (LastMatchWasChoice == true)
				{
					FString Message = FString::Printf(TEXT("Mismatch of CHOOSE and GOTO at line %i (%s)"), i, *Row);
					UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
					Tree.Message = Message;
					return Tree;
				}

				Tree.Nodes[NodeIndex].Edges[EdgeIndex].ChoiceText = GetTextAfterCommand(Row, Type);
				ChoiceNeedsMatch = true;
				
				// UE_LOG(LogTemp, Warning, TEXT("Choice = %s"), *GetTextAfterCommand(Row, Type))
			}
			else
			{
				++EdgeIndex;

				FDialogEdge Edge;
				Edge.ChoiceText = GetTextAfterCommand(Row, Type);
				Tree.Nodes[NodeIndex].Edges.Add(Edge);

				if (DoingChoices)
				{
					ChoiceNeedsMatch = true;
					LastMatchWasChoice = true;
				}

				// UE_LOG(LogTemp, Warning, TEXT("Choice = %s"), *GetTextAfterCommand(Row, Type))
			}
		}
		else if (Type == FULL)
		{
			if (!InEdge || !DoingChoices)
			{
				FString Message = FString::Printf(TEXT("FULL expects to follow a CHOOSE node.  Line %i (%s)"), i, *Row);
				UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
				Tree.Message = Message;
				return Tree;
			}

			Tree.Nodes[NodeIndex].Edges[EdgeIndex].ChoiceTextExtended = GetTextAfterCommand(Row, Type);

			// UE_LOG(LogTemp, Warning, TEXT("Full = %s"), *GetTextAfterCommand(Row, Type))
		}
		else if (Type == SET)
		{
			if (InEdge)
			{
				Tree.Nodes[NodeIndex].Edges[EdgeIndex].Variables.Add(GetTextAfterCommand(Row, Type));
			}
			else
			{
				Tree.Nodes[NodeIndex].Variables.Add(GetTextAfterCommand(Row, Type));
			}

			// UE_LOG(LogTemp, Warning, TEXT("Set (%s) = %s"), InEdge ? TEXT("Edge") : TEXT("Node"), *GetTextAfterCommand(Row, Type))
		}
	}

	
	Tree.Message = TEXT("");
	return Tree;
}

FDialogTree UMLMReader::DEBUG_TestLinearDialog(const FString& FileName)
{
	FDialogTree DialogTree = GenerateDialogTree(TEXT("TEST_LinearScript.txt"));

	return DialogTree;
}
