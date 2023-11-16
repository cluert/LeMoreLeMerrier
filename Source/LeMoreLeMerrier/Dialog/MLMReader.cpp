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

TArray<FDialogEvent> UMLMReader::ParseEventsFromLine(FString Line)
{
	TArray<FDialogEvent> Events;
	int32 Index = 0;
	
	const FString EVENTBRACKET_OPEN = TEXT("[");
	const FString EVENTBRACKET_CLOSE = TEXT("]");

	while (Index < Line.Len())
	{
		// UE_LOG(LogTemp, Warning, TEXT("while loop: index = %i"), Index)
		int32 OpenIndex = Line.Find(EVENTBRACKET_OPEN, ESearchCase::IgnoreCase, ESearchDir::FromStart, Index);
		int32 CloseIndex = Line.Find(EVENTBRACKET_CLOSE, ESearchCase::IgnoreCase, ESearchDir::FromStart, Index);
		bool HasOpen = OpenIndex > -1;
		bool HasClosed = CloseIndex > -1;
		
		if (HasOpen != HasClosed || CloseIndex < OpenIndex)
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing bracket: unpaired bracket at index %i"), HasOpen ? OpenIndex : CloseIndex);
			FDialogEvent NewEvent;
			NewEvent.EventType = EDialogEventType::Line;
			NewEvent.Argument = Line.Mid(Index);
			Events.Add(NewEvent);
			return Events;
		}

		if (HasOpen)
		{
			if (Index != OpenIndex)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Adding Line first..."))
				
				FDialogEvent NewEvent;
				NewEvent.EventType = EDialogEventType::Line;
				NewEvent.Argument = Line.Mid(Index, OpenIndex - Index);
				if (!NewEvent.Argument.TrimStartAndEnd().IsEmpty())
				{
					Events.Add(NewEvent);
				}
			}

			EDialogEventType Type;
			FString Argument;
			FString EventText = Line.Mid(OpenIndex + 1, CloseIndex - OpenIndex - 1);

			if (ParseEventText(EventText, Argument, Type))
			{
				// UE_LOG(LogTemp, Warning, TEXT("parsed event: %i - %s"), Type, *Argument)
				FDialogEvent NewEvent;
				NewEvent.EventType = Type;
				NewEvent.Argument = Argument;
				Events.Add(NewEvent);

				Index = CloseIndex + 1;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("parse event failed:  returning whole line"))
				FDialogEvent NewEvent;
				NewEvent.EventType = Type;
				NewEvent.Argument = Line.Mid(OpenIndex);
				Events.Add(NewEvent);

				return Events;
			}
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("No more events -- returning everything as line"))
			FDialogEvent NewEvent;
			NewEvent.EventType = EDialogEventType::Line;
			NewEvent.Argument = Line.Mid(Index);
			Events.Add(NewEvent);

			Index = Line.Len();
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("Parsed line count = %i"), Events.Num());
	return Events;
}

bool UMLMReader::ParseEventText(const FString& BracketedValue, FString& Value, EDialogEventType& EventType)
{
	FString TrimmedValue = BracketedValue.TrimStartAndEnd();

	const FString EVENTKEY_PAUSE = TEXT("PAUSE");
	const FString EVENTKEY_PLAYRATE = TEXT("SPEED");
	
	if (TrimmedValue.StartsWith(EVENTKEY_PAUSE, ESearchCase::IgnoreCase))
	{
		FString SplitValue = TrimmedValue.Mid(EVENTKEY_PAUSE.Len());
		
		if (!FMath::IsNearlyZero(FCString::Atof(*SplitValue), 0.01f))
		{
			Value = SplitValue;
			EventType = EDialogEventType::Pause;
			return true;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Invalid value for type '%s': '%s'"), *EVENTKEY_PLAYRATE, *SplitValue)
	}
	else if (TrimmedValue.StartsWith(EVENTKEY_PLAYRATE, ESearchCase::IgnoreCase))
	{
		FString SplitValue = TrimmedValue.Mid(EVENTKEY_PLAYRATE.Len());
		
		if (!FMath::IsNearlyZero(FCString::Atof(*SplitValue), 0.01f))
		{
			Value = SplitValue;
			EventType = EDialogEventType::PlaybackSpeed;
			return true;
		}

		UE_LOG(LogTemp, Warning, TEXT("Invalid value for type '%s': '%s'"), *EVENTKEY_PLAYRATE, *SplitValue)
	}
	else
	{
		for (int32 i = EDialogEmotion::None; i < EDialogEmotion::Max; ++i)
		{
			EDialogEmotion Num = (EDialogEmotion) i;
			FText EnumValue;
			UEnum::GetDisplayValueAsText(Num, EnumValue);
			FString MatchStr = EnumValue.ToString();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *MatchStr);
			if (0 == TrimmedValue.Compare(MatchStr, ESearchCase::IgnoreCase))
			{
				Value = FString::FromInt(i);
				EventType = EDialogEventType::Emotion;
				return true;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("No matching event prefix and text did not match emotion: (%s)"), *BracketedValue);
	}

	return false;
}
