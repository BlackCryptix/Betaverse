// Fill out your copyright notice in the Description page of Project Settings.


#include "BetaversePlayerState.h"
#include "Character/CharacterMaster.h"
#include <Runtime\Engine\Public\Net\UnrealNetwork.h>
#include <Runtime\Engine\Classes\Engine\Engine.h>

ABetaversePlayerState::ABetaversePlayerState()
{
}

void ABetaversePlayerState::BeginPlay()
{
	Super::BeginPlay();

	OnPawnSet.AddUniqueDynamic(this, &ABetaversePlayerState::OnPawnChangedCallback);
	if (ACharacterMaster* PlayerCharacter = GetPawn<ACharacterMaster>())
	{
		PlayerCharacter->OnPlayerStateAssigned(this);
	}

	if (HasAuthority())
	{
		PlayerColor = FColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255), FMath::RandRange(0, 255));
	}
}

void ABetaversePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	ABetaversePlayerState* newPlayerState = Cast<ABetaversePlayerState>(PlayerState);
	if (newPlayerState)
	{
		newPlayerState->PersistantCharacterMesh = PersistantCharacterMesh;
	}
}

void ABetaversePlayerState::OnPawnChangedCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	ACharacterMaster* newCharacter = Cast<ACharacterMaster>(NewPawn);
	if (newCharacter)
	{
		newCharacter->OnPlayerStateAssigned(this);
	}
}


void ABetaversePlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABetaversePlayerState, PlayerColor);
}