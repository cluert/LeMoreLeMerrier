// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MLMReader.generated.h"

// notes to self:

// !!! Text files are not automatically packaged by UE when deploying!!!  We need to add them manually to Content
// using the same filepath -- in which case it will work the same as in development (tested on Windows only)

// FYI not using references, so all values are copied everytime they're passed between functions
// so, yknow, keep that in mind if you use this in the future.   not a performance concern since it's done
// literally once a minute or more, but if you had really really large and deeply nested dialog it could
// cause a stutter


USTRUCT(BlueprintType)
struct FDialogLine
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Speaker;
	UPROPERTY(BlueprintReadOnly)
	FString Line;
};

USTRUCT(BlueprintType)
struct FDialogCondition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString KeyName;
	UPROPERTY(BlueprintReadOnly)
	bool bKeyValue;
};

USTRUCT(BlueprintType)
struct FDialogEdge
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString NextNodeId;
	UPROPERTY(BlueprintReadOnly)
	FString ChoiceText;
	UPROPERTY(BlueprintReadOnly)
	FString ChoiceTextExtended;
	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogCondition> Conditions;
	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogLine> Lines;

};

USTRUCT(BlueprintType)
struct FDialogNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Id;
	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogEdge> Edges;
	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogLine> Lines;
};

USTRUCT(BlueprintType)
struct FDialogTree
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogNode> Nodes;
	UPROPERTY(BlueprintReadOnly)
	FString Message;
};

UENUM(BlueprintType)
enum ERowType
{
	NONE = 0 UMETA(Hidden),
	DIALOG UMETA(DisplayName="Dialog"),
	SAY UMETA(DisplayName="Say"),
	LABEL UMETA(DisplayName="Label"),
	GOTO UMETA(DisplayName="GoTo"),
	CHOOSE UMETA(DisplayName="Choose"),
	FULL UMETA(DisplayName="Full"),
	SET UMETA(DisplayName="Set"),
	MAX UMETA(Hidden)
};


/**
 * 
 */
UCLASS()
class LEMORELEMERRIER_API UMLMReader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Dialog)
	static FDialogTree GenerateDialogTree(const FString& FileName);

	UFUNCTION(BlueprintCallable, Category=Dialog)
	static FDialogTree DEBUG_TestLinearDialog(const FString& FileName);

	UFUNCTION(BlueprintCallable, Category=Dialog)
	static bool IsTreeValid(const FDialogTree& Tree)
	{
		return Tree.Message.IsEmpty();
	}
	
private:
	static bool IsWhiteSpace(const char& C)
	{
		return C == ' ' || C == '\t';
	}
	
	static FString FormatRow(const FString& Row)
	{
		return Row.TrimStartAndEnd();
	}

	static FString GetTextAfterCommand(const FString& Row, ERowType Command)
	{
		FText EnumValue;
		UEnum::GetDisplayValueAsText(Command, EnumValue);
		FString FindText = EnumValue.ToString();
		
		int32 Index = Row.Find(FindText, ESearchCase::IgnoreCase);
		if (Index > -1)
		{
			FString Text = Row.Mid(Index + FindText.Len());
			return Text.TrimStartAndEnd();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GetTextAfterCommand: Row did not contain the given command"))
			return FString(TEXT(""));
		}
	}

	static FDialogLine ParseLine(FString Row)
	{
		int32 Index = Row.Find(":", ESearchCase::IgnoreCase, ESearchDir::FromStart, 0);
		FString Name = Row.Left(Index).TrimStartAndEnd();
		FString Line = Row.Mid(Index+1).TrimStartAndEnd();
		FDialogLine Value;
		Value.Speaker = Name;
		Value.Line = Line;
		return Value;
	}

	static ERowType GetRowType(const FString& Row)
	{
		if (Row.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to get row type of empty string"))
			return NONE;
		}
		
		if (Row[0] == '#')
		{
			
			int32 Start;
			for (Start = 1; Start < Row.Len(); ++Start)
			{
				char c = Row[Start];
				if (!IsWhiteSpace(c))
				{
					break;
				}
			}
			int32 End;
			for (End = Start; End < Row.Len(); ++End)
			{
				char c = Row[End];
				if (IsWhiteSpace(c))
				{
					break;
				}
			}
			int32 Count = End - Start;
			FString RawInstruction = Row.Mid(Start, Count);
			FString Instruction = RawInstruction.ToUpper();
			// UE_LOG(LogTemp, Warning, TEXT("Instruction=%s (start=%i count=%i)"), *Instruction,  Start, Count);
			
			for (int32 i = ERowType::NONE; i < ERowType::MAX; ++i)
			{
				ERowType Value = (ERowType) i;
				FText EnumValue;
				UEnum::GetDisplayValueAsText(Value, EnumValue);
				FString MatchStr = EnumValue.ToString();
				
				if (0 == Instruction.Compare(MatchStr, ESearchCase::IgnoreCase))
				{
					return Value;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Unrecognized Instruction: %s"), *RawInstruction);
			return NONE;
		}

		if (Row.Contains(":"))
		{
			// UE_LOG(LogTemp, Warning, TEXT("speaker=%s, line=%s, index=%i"), *Name, *Line, Index);
			return ERowType::DIALOG;
		}

		UE_LOG(LogTemp, Warning, TEXT("Not a valid row: %s "), *Row);
		return ERowType::NONE;
	}
	
	static FDialogNode GetNodeById(TArray<FDialogNode> Nodes, const FString& Id)
	{
		for (int i = 0; i < Nodes.Num(); ++i)
		{
			FDialogNode Node = Nodes[i];
			if (Node.Id.Compare(Id))
			{
				return Node;
			}
		}

		FDialogNode MissingNode;
		MissingNode.Id = TEXT("NodeMissing");
		return MissingNode;
	}
};
