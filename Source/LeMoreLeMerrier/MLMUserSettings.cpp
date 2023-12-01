// Fill out your copyright notice in the Description page of Project Settings.


#include "MLMUserSettings.h"

UMLMUserSettings::UMLMUserSettings()
{
	UMLMUserSettings::Instance = this;
	LoadConfig();
}

UMLMUserSettings* UMLMUserSettings::GetMLMUserSettings()
{
	if (Instance != nullptr)
	{
		Instance = Cast<UMLMUserSettings>(UGameUserSettings::GetGameUserSettings());
	}
	return Instance;
}

float UMLMUserSettings::GetMusicVolume()
{
	return Instance->VolumeMaster * Instance->VolumeMusic;
}

float UMLMUserSettings::GetSFXVolume()
{
	return Instance->VolumeMaster * Instance->VolumeSFX;
}

void UMLMUserSettings::SaveCustomSettings()
{
	SaveConfig();
}
