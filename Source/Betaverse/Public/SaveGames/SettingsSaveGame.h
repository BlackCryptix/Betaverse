// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsSaveGame.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveBoolChanged, bool, Value);

static FString const SettingsSaveGameSlot = "SettingsSaveGameSlot";

USTRUCT(BlueprintType)
struct FSoundSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Master;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Music;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Voice;

};

USTRUCT(BlueprintType)
struct FCharacterSelection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString AssetLink;

};

/**
 * 
 */
UCLASS()
class BETAVERSE_API USettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		FSoundSettings SoundSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		TArray<FCharacterSelection> Characters;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		FString SelectedCharacterUrl;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		bool bUsePushToTalk;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		bool bShowPlayerNames;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		bool bAutoLogin;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		bool bHideChat;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnSaveBoolChanged OnAutoLoginChanged;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnSaveBoolChanged OnShowPlayerNamesChanged;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnSaveBoolChanged OnUsePushToTalkChanged;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnSaveBoolChanged OnHideChatChanged;

public:
	UFUNCTION(BlueprintCallable)
		void SaveAutoLogin(bool Value);

	UFUNCTION(BlueprintCallable)
		void SaveShowPlayerNames(bool Value);

	UFUNCTION(BlueprintCallable)
		void SaveUsePushToTalk(bool Value);

	UFUNCTION(BlueprintCallable)
		void SaveHideChat(bool Value);

	UFUNCTION(BlueprintCallable)
		void SaveMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable)
		void SaveSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable)
		void SaveMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable)
		void SaveVoiceVolume(float Volume);

	UFUNCTION(BlueprintCallable)
		void ChangeSelectedCharacter(FString Url);

	UFUNCTION(BlueprintCallable)
		void AddCharacter(FCharacterSelection Character);

	UFUNCTION(BlueprintCallable)
		void RemoveCharacter(int Index);

	UFUNCTION()
		bool Save();
};
