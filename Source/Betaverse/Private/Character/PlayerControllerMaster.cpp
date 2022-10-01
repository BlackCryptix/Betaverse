// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerControllerMaster.h"
#include "BetaverseState.h"
#include "BetaversePlayerState.h"
#include "Components/WidgetInteractionComponent.h"

APlayerControllerMaster::APlayerControllerMaster()
{
	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComp"));
	WidgetInteractionComp->InteractionSource = EWidgetInteractionSource::CenterScreen;
	WidgetInteractionComp->SetupAttachment(GetRootComponent());
}

void APlayerControllerMaster::SendChatMessage_Implementation(const FString& PlayerName, const FText& Message)
{
	ABetaverseState* betaverseState = GetWorld()->GetGameState<ABetaverseState>();
	betaverseState->BroadcastChatMessage(PlayerName, Message);
}


void APlayerControllerMaster::ReceiveTextMessage_Implementation(const ABetaversePlayerState* Sender, const FText& Message, bool bPrivateMessage)
{
	OnChatMessageReceived.Broadcast(Sender, Message, bPrivateMessage);
}
