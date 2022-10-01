// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerMaster.generated.h"

class ABetaversePlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnChatMessageReceived, const ABetaversePlayerState*, Sender, const FText&, Message, const bool, bPrivateMessage);

/**
 * 
 */
UCLASS()
class BETAVERSE_API APlayerControllerMaster : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControllerMaster();

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChatMessageReceived OnChatMessageReceived;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Interaction")
		class UWidgetInteractionComponent* WidgetInteractionComp;


public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void SendChatMessage(const FString& PlayerName, const FText& Message);

	UFUNCTION(Client, Reliable)
		void ReceiveTextMessage(const ABetaversePlayerState* Sender, const FText& Message, bool bPrivateMessage);
};
