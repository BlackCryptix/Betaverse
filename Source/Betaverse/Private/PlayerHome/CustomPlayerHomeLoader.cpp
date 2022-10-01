// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHome/CustomPlayerHomeLoader.h"
#include "BetaverseInstance.h"
#include "SaveGames/HomeSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerHome/PlotLandscape.h"
#include "Engine/LevelStreamingDynamic.h"
#include <Runtime\Engine\Public\Net\UnrealNetwork.h>
#include <Runtime\Engine\Classes\Engine\Engine.h>

// Sets default values
ACustomPlayerHomeLoader::ACustomPlayerHomeLoader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACustomPlayerHomeLoader::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		UBetaverseInstance* BetaverseInstance = GetGameInstance<UBetaverseInstance>();
		FString CustomLevelName = BetaverseInstance->GetCustomLevelName();

		//load save game which hold the level actors and positions
		UHomeSaveGame* levelSave = Cast<UHomeSaveGame>(UGameplayStatics::LoadGameFromSlot(CustomLevelName, 0));
		LevelActorsToLoad = levelSave->SavedHomeActors;

		bool wasLoadedSuccessful;
		ULevelStreamingDynamic::LoadLevelInstance(this, LevelActorsToLoad.BaseLevelName, FVector::ZeroVector, FRotator::ZeroRotator, wasLoadedSuccessful);

		int numOfActors = levelSave->SavedHomeActors.Actors.Num();

		//load saved assets
		for (int i = 0; i < numOfActors; i++)
		{
			AActor* actorToSpawn = GetWorld()->SpawnActorDeferred<AActor>(LevelActorsToLoad.Actors[i], LevelActorsToLoad.ActorTransforms[i], nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (actorToSpawn)
			{
				UGameplayStatics::FinishSpawningActor(actorToSpawn, LevelActorsToLoad.ActorTransforms[i]);
			}
		}

		//Spawn Landscape
		if (LevelActorsToLoad.PlotSize.SizeSquared() > 0.0f)
		{
			const FTransform plotSpawn = FTransform(100.0f * FVector(-LevelActorsToLoad.PlotSize.X / 2.0f, -LevelActorsToLoad.PlotSize.Y / 2.0f, 0.0f));
			APlotLandscape* plotLandscape = GetWorld()->SpawnActorDeferred<APlotLandscape>(APlotLandscape::StaticClass(), plotSpawn, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (plotLandscape)
			{
				plotLandscape->LandscapeMaterial = LevelActorsToLoad.PlotMaterial;
				plotLandscape->WallMaterial = LevelActorsToLoad.PlotWallMaterial;
				UGameplayStatics::FinishSpawningActor(plotLandscape, plotSpawn);
				plotLandscape->BuildLandscape(LevelActorsToLoad.PlotSize);
			}
		}	
	}
}

// Called every frame
void ACustomPlayerHomeLoader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACustomPlayerHomeLoader::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACustomPlayerHomeLoader, LevelActorsToLoad);
}