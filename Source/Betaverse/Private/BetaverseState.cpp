// Fill out your copyright notice in the Description page of Project Settings.


#include "BetaverseState.h"
#include "Character/PlayerControllerMaster.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetTextLibrary.h"


void ABetaverseState::BroadcastChatMessage(const FString& PlayerName, const FText& Message)
{
	ABetaversePlayerState* Sender;
	//find player state of player who sent the message
	for (APlayerState* player : PlayerArray)
	{
		if (player->GetPlayerName() == PlayerName)
		{
			Sender = (ABetaversePlayerState*)player;

			FString messageAsSting = Message.ToString();
			//check if the message is a private message
			if (messageAsSting.StartsWith("/"))
			{
				FString left, right;
				messageAsSting.Split(" ", &left, &right);
				//find private message receiver
				for (APlayerState* receiver : PlayerArray)
				{
					if (left.Contains(receiver->GetPlayerName()))
					{
						((APlayerControllerMaster*)receiver->GetPlayerController())->ReceiveTextMessage(Sender, FText::FromString(right), true);
						((APlayerControllerMaster*)player->GetPlayerController())->ReceiveTextMessage(Sender, FText::FromString(right), true);
						return;
					}
				}
			}
			else
			{
				for (APlayerState* receiver : PlayerArray)
				{
					((APlayerControllerMaster*)receiver->GetPlayerController())->ReceiveTextMessage(Sender, Message, false);
				}
			}
			return;
		}
	}
}