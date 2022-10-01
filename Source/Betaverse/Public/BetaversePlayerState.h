// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BetaversePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BETAVERSE_API ABetaversePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABetaversePlayerState();

	UPROPERTY()
		class USkeletalMesh* PersistantCharacterMesh;

private:

	UPROPERTY(Replicated)
		FColor PlayerColor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CopyProperties(APlayerState* PlayerState);

public:
	UFUNCTION(BlueprintCallable, BLueprintPure)
		FColor GetPlayerColor() { return PlayerColor; };

private:
	UFUNCTION()
		void OnPawnChangedCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

};
