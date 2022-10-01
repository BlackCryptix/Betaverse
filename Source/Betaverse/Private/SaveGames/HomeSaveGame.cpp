// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGames/HomeSaveGame.h"
#include "Kismet/GameplayStatics.h"

bool UHomeSaveGame::SaveLevel(FSavedHomeActors LevelActors, FString LevelName)
{
	SavedHomeActors = LevelActors;
	return UGameplayStatics::SaveGameToSlot(this, LevelName, 0);
}
