// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HomeSaveGame.generated.h"


USTRUCT(BlueprintType)
struct FSavedHomeActors
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString BaseLevelName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector2D PlotSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInstance* PlotMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInstance* PlotWallMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<TSubclassOf<AActor>> Actors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FTransform> ActorTransforms;

	FSavedHomeActors()
	{
		PlotSize = FVector2D::ZeroVector;
		PlotMaterial = nullptr;
		PlotWallMaterial = nullptr;
	}
};

/**
 * 
 */
UCLASS()
class BETAVERSE_API UHomeSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, Category = "HomeSaveGame")
		FSavedHomeActors SavedHomeActors;

public:

	UFUNCTION(BlueprintCallable)
		bool SaveLevel(FSavedHomeActors LevelActors, FString LevelName);
};
