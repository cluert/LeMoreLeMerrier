// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MLMUserSettings.generated.h"

UCLASS(config=GameUserSettings, BlueprintType)
class LEMORELEMERRIER_API UMLMUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UMLMUserSettings();
	static inline UMLMUserSettings* Instance = nullptr;

	UFUNCTION(BlueprintPure, BlueprintCallable, meta=(CompactNodeTitle = "Settings"))
	static UMLMUserSettings* GetMLMUserSettings();

	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (CompactNodeTitle = "Music Volume"))
	static float GetMusicVolume();

	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (CompactNodeTitle = "SFX Volume"))
	static float GetSFXVolume();

	UFUNCTION(BlueprintCallable)
	void SaveCustomSettings();

	// User settings:
	UPROPERTY(Config, BlueprintReadWrite)
	FText PlayerName;

	UPROPERTY(Config, BlueprintReadWrite)
	float VolumeMaster = 1.0f;

	UPROPERTY(Config, BlueprintReadWrite)
	float VolumeMusic = 1.0f;

	UPROPERTY(Config, BlueprintReadWrite)
	float VolumeSFX = 1.0f;

	UPROPERTY(Config, BlueprintReadWrite)
	bool bInvertScrollDirection = false;
};