// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "LevelInfoLibrary.generated.h"


USTRUCT(BlueprintType)
struct FLevelInfoRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName DisplayName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UTexture2D* LevelPreview;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName LevelName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector2D Size;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int RecommendedPlayers;

	FLevelInfoRow() 
	{
		LevelPreview = nullptr;
		Size = FVector2D(0.0f);
		RecommendedPlayers = 4;
	};

	bool operator==(const FLevelInfoRow& otherStruct)
	{
		return otherStruct.DisplayName == DisplayName;
	};
};

/**
 * 
 */
UCLASS()
class BETAVERSE_API ULevelInfoLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "LevelInfoRow")
		static FName GetDisplayName(FLevelInfoRow LevelInfoRow) { return LevelInfoRow.DisplayName; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "LevelInfoRow")
		static UTexture2D* GetPreviewImage(FLevelInfoRow LevelInfoRow) { return LevelInfoRow.LevelPreview; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "LevelInfoRow")
		static FName GetLevelName(FLevelInfoRow LevelInfoRow) { return LevelInfoRow.LevelName; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "LevelInfoRow")
		static int GetRecommendedPlayers(FLevelInfoRow LevelInfoRow) { return LevelInfoRow.RecommendedPlayers; };
};
