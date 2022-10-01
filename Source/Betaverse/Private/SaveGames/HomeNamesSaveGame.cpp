// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGames/HomeNamesSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UHomeNamesSaveGame::AddNewLevelLoadInfo(const FLevelInfoRow& HomeLevelLoadInfo)
{
	HomeLevelsLoadInfo.Remove(HomeLevelLoadInfo);
	HomeLevelsLoadInfo.Add(HomeLevelLoadInfo);
	Save();
}

void UHomeNamesSaveGame::RemoveLevelLoadInfo(const FLevelInfoRow& HomeLevelLoadInfo)
{
	HomeLevelsLoadInfo.Remove(HomeLevelLoadInfo);
	UGameplayStatics::DeleteGameInSlot(HomeLevelLoadInfo.DisplayName.ToString(), 0);
	Save();
}

bool UHomeNamesSaveGame::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, HomeNameSaveGameSlot, 0);
}
