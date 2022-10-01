// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LevelInfoLibrary.h"
#include "HomeNamesSaveGame.generated.h"

static FString const HomeNameSaveGameSlot = "HomeNameSaveGameSlot";


/**
 * 
 */
UCLASS()
class BETAVERSE_API UHomeNamesSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, Category = "HomeNamesSaveGame")
		TArray<FLevelInfoRow> HomeLevelsLoadInfo;

public:

	UFUNCTION(BlueprintCallable)
		void AddNewLevelLoadInfo(const FLevelInfoRow& HomeLevelLoadInfo);

	UFUNCTION(BlueprintCallable)
		void RemoveLevelLoadInfo(const FLevelInfoRow& HomeLevelLoadInfo);

	UFUNCTION(BlueprintCallable)
		bool Save();

};
