// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGames/SettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"

void USettingsSaveGame::SaveAutoLogin(bool Value)
{
	bAutoLogin = Value;
	OnAutoLoginChanged.Broadcast(bAutoLogin);
	Save();
}

void USettingsSaveGame::SaveShowPlayerNames(bool Value)
{
	bShowPlayerNames = Value;
	OnShowPlayerNamesChanged.Broadcast(bShowPlayerNames);
	Save();
}

void USettingsSaveGame::SaveUsePushToTalk(bool Value)
{
	bUsePushToTalk = Value;
	OnUsePushToTalkChanged.Broadcast(bUsePushToTalk);
	Save();
}

void USettingsSaveGame::SaveHideChat(bool Value)
{
	bHideChat = Value;
	OnHideChatChanged.Broadcast(bHideChat);
	Save();
}

void USettingsSaveGame::SaveMasterVolume(float Volume)
{
	SoundSettings.Master = Volume;
	Save();
}

void USettingsSaveGame::SaveSFXVolume(float Volume)
{
	SoundSettings.SFX = Volume;
	Save();
}

void USettingsSaveGame::SaveMusicVolume(float Volume)
{
	SoundSettings.Music = Volume;
	Save();
}

void USettingsSaveGame::SaveVoiceVolume(float Volume)
{
	SoundSettings.Voice = Volume;
	Save();
}

void USettingsSaveGame::ChangeSelectedCharacter(FString Url)
{
	SelectedCharacterUrl = Url;
	Save();
}

void USettingsSaveGame::AddCharacter(FCharacterSelection Character)
{
	Characters.Add(Character);
	Save();
}

void USettingsSaveGame::RemoveCharacter(int Index)
{
	Characters.RemoveAt(Index);
	Save();
}

bool USettingsSaveGame::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, SettingsSaveGameSlot, 0);
}
