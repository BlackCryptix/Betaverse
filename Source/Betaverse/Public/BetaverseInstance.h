// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "Online/CoreOnline.h"
#include "BetaverseInstance.generated.h"

class IVoiceChatUser;
class ACharacterMaster;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsInValidSessionChanged, bool, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginFinished, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateSessionFinished, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroySessionFinished, bool, bWasSuccessful);

/** Notify with sessions when find sessions finished */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinSessionFinished, bool, bWasSuccessful);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetFriendsFinished, bool, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsOnlineChanged, bool, IsOnline);


UCLASS()
class BETAVERSE_API UExtendedBlueprintSessionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Online|Session")
		static bool GetIsDedicatedServer(FBlueprintSessionResult Session) { return Session.OnlineResult.Session.SessionSettings.bIsDedicated; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Online|Session")
		static bool GetIsPrivate(FBlueprintSessionResult Session) { return Session.OnlineResult.Session.SessionSettings.NumPrivateConnections > 0; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Online|Session")
		static bool GetLevelName(FBlueprintSessionResult Session, FString& LevelName) { return Session.OnlineResult.Session.SessionSettings.Get(SETTING_MAPNAME, LevelName); };

};

/**
 * 
 */
UCLASS()
class BETAVERSE_API UBetaverseInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly)
		class USettingsSaveGame* Settings;

	UPROPERTY(BlueprintReadOnly)
		class UHomeNamesSaveGame* HomeNames;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FBlueprintFindSessionsResultDelegate OnBlueprintSessionsResultReceived;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnLoginFinished OnLoginFinished;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnGetFriendsFinished OnGetFriendsFinished;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnDestroySessionFinished OnDestroySessionFinished;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnCreateSessionFinished OnCreateSessionFinished;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnIsOnlineChanged OnIsOnlineChanged;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnJoinSessionFinished OnJoinSessionFinished;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnIsInValidSessionChanged OnIsInValidSessionChanged;

protected:
	class IOnlineSubsystem* OnlineSubsystem;

private:
	TSharedPtr<FOnlineSessionSearch> SearchSettings;

	UPROPERTY()
		FString CustomLevelName = "TestHome";

	UPROPERTY()
		ACharacterMaster* LocalPlayerCharacter;

	UPROPERTY()
		bool bIsInValidSession = false;
	void SetIsInValidSession(bool Value) { bIsInValidSession = Value; OnIsInValidSessionChanged.Broadcast(Value); };

public:
	UBetaverseInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintPure, BlueprintCallable)
		FString GetCustomLevelName() { return CustomLevelName; };

	//EOS Networking functions

	UFUNCTION(BlueprintCallable)
		void CreateSession(const FName& LevelName, const FString& DisplayLevelName = "", const int& NumberOfPlayers = 4, const bool& IsPrivate = false);
	void CreateSessionCallback(FName SessionName, bool bWasSuccessful);

	/**
	* Call to Login into the EOS online Subsystem
	*/
	UFUNCTION(BlueprintCallable)
		void Login();
	void LoginCallback(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	/**
	* Call to Logout from the EOS Subsystem
	*/
	UFUNCTION(BlueprintCallable)
		void Logout();
	void LogoutCallback(int32 LocalUserNum, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
		void DestroySession();
	void DestroySessionCallback(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
		void GetFriends();
	void GetFriendsCallback(int LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	UFUNCTION(BlueprintCallable)
		void FindSessions();
	void FindSessionsCallback(bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, meta = (DisplayName="JoinSession"))
		void K2_JoinSession(FBlueprintSessionResult SessionToJoin) { JoinMetaverse(FName("Lobby"), SessionToJoin.OnlineResult); }
	void JoinMetaverse(FName SessionName, const FOnlineSessionSearchResult& SessionToJoin);
	void JoinMetaverseCallback(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
		void ServerTravel(FString NewLevelName, const FString& DisplayLevelName = "");

	UFUNCTION(BlueprintCallable)
		void QuitLobby();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsOnline();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsSessionPresent() { return bIsInValidSession; };

	void LoginStatusChangedCallback(int32 PlayerNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId);

	//Intilization Functions
	UFUNCTION(BlueprintCallable)
		void CreateDefaultSession();

	UFUNCTION(BlueprintCallable)
		void LoadGameSettings();

#pragma region Voice Interface
	/**
	 * Mute a specific network player
	 *
	 * @param PlayerId Local player who wants to mute another player
	 * @param MutePlayerId NetworkPlayer which is going to be muted
	 * @param bMute True to mute, false to unmute
	 */
	UFUNCTION(BlueprintCallable, Category = "Voice", meta = (DisplayName = "MutePlayer"))
		void K2_MutePlayer(const FUniqueNetIdRepl& PlayerId, const FUniqueNetIdRepl& MutePlayerId, bool bMute);
	void MutePlayer(const FUniqueNetId& PlayerId, const FUniqueNetId& MutePlayerId, bool bMute);

	/**
	 * Mute the input device of the given local player
	 *
	 * @param NetPlayerId Local player to mute/unmute
	 * @param bMute True to mute, false to unmute
	 */
	UFUNCTION(BlueprintCallable, Category = "Voice", meta = (DisplayName = "Mute"))
		void K2_Mute(const FUniqueNetIdRepl& PlayerId, bool bMute);
	void Mute(const FUniqueNetId& PlayerId, bool bMute);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice")
		TArray<FString> GetVoicePlayerNames(const FUniqueNetIdRepl& PlayerId, const FString& ChannelName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice")
		TArray<FString> GetVoiceChannels(const FUniqueNetIdRepl& PlayerId);

	UFUNCTION(BlueprintCallable)
		void UpdatePlayerVoicePosition(ACharacterMaster* PlayerCharacter);

	/**
	* Used to keep track of the locally controlled player character. Needs to be set to apply positions and distances between players for voice comunication.
	* 
	* @param PlayerCharacter the player character which is controlled locally by the user
	*/
	UFUNCTION(BlueprintCallable)
		void SetLocalPlayerCharacter(ACharacterMaster* PlayerCharacter) { LocalPlayerCharacter = PlayerCharacter; }

private:
	/**
	* Get a local voice user by his net id
	* 
	* @param PlayerId
	* @param OutVoiceUser return value of the ivoicechatuser which was requested
	* 
	* @return bool if the IVoiceChatUser get be retrieved
	*/
	bool GetVoiceUser(const FUniqueNetId& PlayerId, IVoiceChatUser*& OutVoiceUser);
#pragma endregion

	
};
