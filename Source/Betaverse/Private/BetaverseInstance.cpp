// Fill out your copyright notice in the Description page of Project Settings.


#include "BetaverseInstance.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/VoiceInterface.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SaveGames/SettingsSaveGame.h"
#include "SaveGames/HomeNamesSaveGame.h"
#include "Character/CharacterMaster.h"
#include "GameFramework/PlayerState.h"
#include "OnlineSubsystemEOS.h"
#include "VoiceChat.h"



UBetaverseInstance::UBetaverseInstance()
{
}

void UBetaverseInstance::Init()
{
	Super::Init();
	LoadGameSettings();

	OnlineSubsystem = IOnlineSubsystem::Get();

	if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
	{
		Identity->OnLoginStatusChangedDelegates->AddUObject(this, &UBetaverseInstance::LoginStatusChangedCallback);
	}
}

void UBetaverseInstance::CreateSession(const FName& LevelName, const FString& DisplayLevelName, const int& NumberOfPlayers, const bool& IsPrivate)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsDedicated = false;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bIsLANMatch = false;
			
			if (IsPrivate)
				SessionSettings.NumPrivateConnections = NumberOfPlayers;
			else
				SessionSettings.NumPublicConnections = NumberOfPlayers;
			
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bAllowJoinViaPresence = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.bUseLobbiesIfAvailable = true;
			SessionSettings.bUseLobbiesVoiceChatIfAvailable = true;
			SessionSettings.Set(SEARCH_KEYWORDS, FString("Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings.Set(SETTING_MAPNAME, LevelName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

			CustomLevelName = DisplayLevelName;

			if (bIsInValidSession)
			{
				Session->UpdateSession("SessionName", SessionSettings);
				ServerTravel(LevelName.ToString(), DisplayLevelName);
			}
			else
			{
				Session->OnCreateSessionCompleteDelegates.AddUObject(this, &UBetaverseInstance::CreateSessionCallback);
				Session->CreateSession(0, FName("SessionName"), SessionSettings);
			}
		}
	}
}

void UBetaverseInstance::CreateSessionCallback(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
		{
			Session->ClearOnCreateSessionCompleteDelegates(this);

			if (bWasSuccessful)
			{
				FOnlineSessionSettings* sessionSettings = Session->GetSessionSettings(SessionName);
				FString LevelName;
				if (sessionSettings->Get(SETTING_MAPNAME, LevelName))
				{
					SetIsInValidSession(true);
					UGameplayStatics::OpenLevel(GetWorld(), FName(LevelName), true, FString("?listen"));
				}
			}
		}
	}

	OnCreateSessionFinished.Broadcast(bWasSuccessful);
}

void UBetaverseInstance::Login()
{
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString("accountportal");

			Identity->OnLoginCompleteDelegates->AddUObject(this, &UBetaverseInstance::LoginCallback);
			Identity->Login(0, Credentials);
		}
	}
}

void UBetaverseInstance::LoginCallback(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	}

	OnLoginFinished.Broadcast(bWasSuccessful);
}

void UBetaverseInstance::Logout()
{
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			Identity->OnLogoutCompleteDelegates->AddUObject(this, &UBetaverseInstance::LogoutCallback);
			Identity->Logout(0);
		}
	}
}

void UBetaverseInstance::LogoutCallback(int32 LocalUserNum, bool bWasSuccessful)
{
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			Identity->ClearOnLogoutCompleteDelegates(0, this);
		}
	}
}

void UBetaverseInstance::DestroySession()
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
		{
			Session->OnDestroySessionCompleteDelegates.AddUObject(this, &UBetaverseInstance::DestroySessionCallback);
			Session->DestroySession(FName("SessionName"));
		}
	}
}

void UBetaverseInstance::DestroySessionCallback(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
		{
			Session->ClearOnDestroySessionCompleteDelegates(this);
			SetIsInValidSession(false);
		}
	}

	OnDestroySessionFinished.Broadcast(bWasSuccessful);
}

void UBetaverseInstance::GetFriends()
{
	if (OnlineSubsystem)
	{
		if (IOnlineFriendsPtr Friends = OnlineSubsystem->GetFriendsInterface())
		{
			Friends->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UBetaverseInstance::GetFriendsCallback));
		}
	}
}

void UBetaverseInstance::GetFriendsCallback(int LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (bWasSuccessful)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr Friends = OnlineSubsystem->GetFriendsInterface())
			{
				TArray<TSharedRef<FOnlineFriend>> FriendList;
				if (Friends->GetFriendsList(0, ListName, FriendList))
				{
					for (auto Friend : FriendList)
					{
						UE_LOG(LogTemp, Warning, TEXT("The Actor's name is %s"), *Friend.Get().GetRealName());
					}
				}
			}
		}
	}
}

void UBetaverseInstance::FindSessions()
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
		{
			SearchSettings = MakeShareable(new FOnlineSessionSearch());
			SearchSettings->MaxSearchResults = 100;
			SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("Lobby"), EOnlineComparisonOp::Equals);
			SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

			Session->OnFindSessionsCompleteDelegates.AddUObject(this, &UBetaverseInstance::FindSessionsCallback);
			Session->FindSessions(0, SearchSettings.ToSharedRef());
		}
	}
}

void UBetaverseInstance::FindSessionsCallback(bool bWasSuccessful)
{
	//convert to blueprint array
	TArray<FBlueprintSessionResult> Results;
	if (bWasSuccessful)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
			{
				Session->ClearOnFindSessionsCompleteDelegates(this);
			}
		}

		for (auto& SearchResult : SearchSettings->SearchResults)
		{
			FBlueprintSessionResult BPResult;
			BPResult.OnlineResult = SearchResult;
			Results.Add(BPResult);
		}
	}
	
	//call blueprint delegate to display sessions in ui
	OnBlueprintSessionsResultReceived.Broadcast(Results);
}

void UBetaverseInstance::JoinMetaverse(FName SessionName, const FOnlineSessionSearchResult& SessionToJoin)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
		{
			Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UBetaverseInstance::JoinMetaverseCallback);
			Session->JoinSession(0, SessionName, SearchSettings->SearchResults[0]);
		}
	}
}

void UBetaverseInstance::JoinMetaverseCallback(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface())
		{
			Session->ClearOnJoinSessionCompleteDelegates(this);

			if (Result == EOnJoinSessionCompleteResult::Type::Success)
			{
				FString ConnectionInfo = FString();
				Session->GetResolvedConnectString(SessionName, ConnectionInfo);
				if (!ConnectionInfo.IsEmpty())
				{
					if (APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
					{
						playerController->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
						SetIsInValidSession(true);
					}
				}
			}
		}
		OnJoinSessionFinished.Broadcast(true);
	}
	else
	{
		OnJoinSessionFinished.Broadcast(false);
	}
}

void UBetaverseInstance::ServerTravel(FString NewLevelName, const FString& DisplayLevelName)
{
	CustomLevelName = DisplayLevelName;
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), FString("servertravel ").Append(NewLevelName));
}

void UBetaverseInstance::QuitLobby()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_Client)
	{
		CreateDefaultSession();
	}
	else if (GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		DestroySession();
	}
}

bool UBetaverseInstance::IsOnline()
{
	return OnlineSubsystem->GetIdentityInterface()->GetLoginStatus(0) == ELoginStatus::Type::LoggedIn;
}

void UBetaverseInstance::LoginStatusChangedCallback(int32 PlayerNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId)
{
	OnIsOnlineChanged.Broadcast(ELoginStatus::Type::LoggedIn == NewStatus);
}

void UBetaverseInstance::CreateDefaultSession()
{
	if (IsOnline())
	{
		CreateSession(FName("DefaultPlayerHome"), "", 4, true);
	}
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("DefaultPlayerHome"), true);
	}
}

void UBetaverseInstance::LoadGameSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SettingsSaveGameSlot, 0))
	{
		Settings = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SettingsSaveGameSlot, 0));
	}
	else
	{
		Settings = Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
	}

	if (UGameplayStatics::DoesSaveGameExist(HomeNameSaveGameSlot, 0))
	{
		HomeNames = Cast<UHomeNamesSaveGame>(UGameplayStatics::LoadGameFromSlot(HomeNameSaveGameSlot, 0));
	}
	else
	{
		HomeNames = Cast<UHomeNamesSaveGame>(UGameplayStatics::CreateSaveGameObject(UHomeNamesSaveGame::StaticClass()));
	}
}

void UBetaverseInstance::K2_MutePlayer(const FUniqueNetIdRepl& PlayerId, const FUniqueNetIdRepl& MutePlayerId, bool bMute)
{
	MutePlayer(*PlayerId.GetUniqueNetId(), *MutePlayerId.GetUniqueNetId(), bMute);

	UE_LOG(LogTemp, Warning, TEXT("The Actor's name is %s"), *MutePlayerId.GetUniqueNetId()->ToString());
}

void UBetaverseInstance::MutePlayer(const FUniqueNetId& PlayerId, const FUniqueNetId& MutePlayerId, bool bMute)
{
	IVoiceChatUser* voiceUser;
	if (GetVoiceUser(PlayerId, voiceUser))// && GetVoiceUser(MutePlayerId, voiceUserMute))
	{
		FString left, right;
		MutePlayerId.ToString().Split(FString("|"), &left, &right);
		voiceUser->SetPlayerMuted(*right, bMute);
	}
}

void UBetaverseInstance::K2_Mute(const FUniqueNetIdRepl& PlayerId, bool bMute)
{
	Mute(*PlayerId.GetUniqueNetId().Get(), bMute);
}

void UBetaverseInstance::Mute(const FUniqueNetId& PlayerId, bool bMute)
{
	IVoiceChatUser* voiceUser;
	if (GetVoiceUser(PlayerId, voiceUser))
	{
		voiceUser->SetAudioInputDeviceMuted(bMute);
	}
}

TArray<FString> UBetaverseInstance::GetVoicePlayerNames(const FUniqueNetIdRepl& PlayerId, const FString& ChannelName)
{
	IVoiceChatUser* voiceUser;
	if (GetVoiceUser(*PlayerId.GetUniqueNetId().Get(), voiceUser))
	{
		return voiceUser->GetPlayersInChannel(ChannelName);
	}
	return TArray<FString>();
}

TArray<FString> UBetaverseInstance::GetVoiceChannels(const FUniqueNetIdRepl& PlayerId)
{
	IVoiceChatUser* voiceUser;
	if (GetVoiceUser(*PlayerId.GetUniqueNetId().Get(), voiceUser))
	{	
		return voiceUser->GetChannels();
	}
	return TArray<FString>();
}

void UBetaverseInstance::UpdatePlayerVoicePosition(ACharacterMaster* PlayerCharacter)
{
	if (IsValid(LocalPlayerCharacter))
	{
		APlayerState* pState = PlayerCharacter->GetPlayerState();
		
		if (IsValid(pState))
		{
			//Todo set 3d voice position for positional talk
			/*IVoiceChatUser* voiceUser;
			if (GetVoiceUser(*pState->GetUniqueId().GetUniqueNetId(), voiceUser))
			{
				UE_LOG(LogTemp, Warning, TEXT("3"));
				for (FString channel : voiceUser->GetChannels())
				{
					voiceUser->Set3DPosition(channel, PlayerCharacter->GetActorLocation(), LocalPlayerCharacter->GetActorLocation(), LocalPlayerCharacter->GetActorForwardVector(), LocalPlayerCharacter->GetActorUpVector());
				}
			}*/

			IVoiceChatUser* localVoiceUser;
			APlayerState* pState2 = LocalPlayerCharacter->GetPlayerState();
			if (pState2)
			{
				FUniqueNetIdPtr idPtr = pState2->GetUniqueId().GetUniqueNetId();
				if (idPtr.IsValid())
				{
					if (GetVoiceUser(*idPtr, localVoiceUser))
					{
						FString left, right;
						pState->GetUniqueId().GetUniqueNetId()->ToString().Split(FString("|"), &left, &right);

						//calculate volume based on distance to player
						//Todo: volume seems to only affect if the player voice is on or off (0 or >0)
						float volume = 1.0f - FVector::DistSquared(LocalPlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation()) / FMath::Square(PlayerCharacter->GetSpeakingDistance());
						localVoiceUser->SetPlayerVolume(right, FMath::Clamp(volume, 0.0f, 1.0f));
					}
				}
			}
		}
	}
}

bool UBetaverseInstance::GetVoiceUser(const FUniqueNetId& PlayerId, IVoiceChatUser*& OutVoiceUser)
{
	FOnlineSubsystemEOS* eosSubsystem = (FOnlineSubsystemEOS*)OnlineSubsystem;

	if (&PlayerId && eosSubsystem)
	{
		OutVoiceUser = eosSubsystem->GetVoiceChatUserInterface(PlayerId);

		if (OutVoiceUser)
		{
			return true;
		}
	}
	return false;
}
