// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveGames/HomeSaveGame.h"
#include "CustomPlayerHomeLoader.generated.h"

UCLASS()
class BETAVERSE_API ACustomPlayerHomeLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomPlayerHomeLoader();

	UPROPERTY(Replicated)
		FSavedHomeActors LevelActorsToLoad;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
