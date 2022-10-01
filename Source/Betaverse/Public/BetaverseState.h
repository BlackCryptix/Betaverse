// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BetaverseState.generated.h"

/**
 * 
 */
UCLASS()
class BETAVERSE_API ABetaverseState : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION()
		void BroadcastChatMessage(const FString& PlayerName, const FText& Message);
};
