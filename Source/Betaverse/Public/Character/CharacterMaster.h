// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterMaster.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCanInteractChanged,bool, CanInteract);

class USpringArmComponent;
class UCameraComponent;
class APickUpObject;
class UBetaverseInstance;

UCLASS()
class BETAVERSE_API ACharacterMaster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterMaster();
	
	//Scene Components
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character")
		class UReadyPlayerMeActorComponent* ReadyPlayerMeComp;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Player Audio")
		class UNiagaraComponent* VOIPRangeIndicator;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character")
		class UWidgetComponent* PlayerNameComp;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Camera")
		USpringArmComponent* TPCameraArm;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Camera")
		USpringArmComponent* TPCameraArmRight;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Camera")
		USpringArmComponent* FPCameraArm;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Camera")
		UCameraComponent* TPCamera;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Camera")
		UCameraComponent* FPCamera;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Player Audio")
		TArray<class USoundAttenuation*> SpeakDistances;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnCanInteractChanged OnCanInteractChanged;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly, Category = "Player Audio")
		int SpeakDistanceIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		class USkeletalMesh* DefaultMesh;

	UPROPERTY(BlueprintReadOnly)
		APickUpObject* CarryItem;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
		APickUpObject* GetCarryItem() { return CarryItem; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetSpeakingDistance() { return SpeakDistances[SpeakDistanceIndex]->Attenuation.FalloffDistance; };

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
		void SetCarryItem(APickUpObject* Item);

private:

	UPROPERTY()
		bool bFirstPerson = false;

	UPROPERTY(Replicated)
		bool bIsRuning = false;

	UPROPERTY()
		AActor* InteractionActor;

	UPROPERTY()
		FHitResult CurrentInteractionHit;

	UPROPERTY()
		UBetaverseInstance* BetaverseInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsFirstPerson() { return bFirstPerson; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsRunning() { return bIsRuning; };

	/**
	* Returns the offset for the camera arm in case the current control is in 3d person
	* 
	* @return Offset
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetInteractTraceOffset();

	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool GetInteractionHitResult(FHitResult& InteractionHit) { InteractionHit = CurrentInteractionHit; return IsValid(InteractionActor); };

	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool GetUVHitResult(FVector2D& InteractionHit);

	UFUNCTION(BlueprintPure, BlueprintCallable)
		UCameraComponent* GetActiveCamera() { return bFirstPerson ? FPCamera : TPCamera; }

	/**
	*	Used to make a RPC call when interacting with world actors
	* 
	* @param CallbackActor The actor which the player interacted with (should always implement the IInteract interface)
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction", meta = (Keywords = "Actor Interaction", DefaultToSelf = "CallbackActor"))
		void ServerInteract(AActor* CallbackActor);

	/**
	*	Used to make a RPC call when stopping interacting with world actors
	*
	* @param CallbackActor The actor which the player interacted with (should always implement the IInteract interface)
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction", meta = (Keywords = "Actor Interaction", DefaultToSelf = "CallbackActor"))
		void ServerStopInteract(AActor* CallbackActor);

	/**
	* Used to Replicate a button press on a 3D Widget
	* 
	* @param WidgetOwner Actor who owns the 3d Widget
	* @param ButtonIndex Index of the Button which was pressed inside the widget
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction", meta = (Keywords = "Actor Interaction", DefaultToSelf = "WidgetOwner"))
		void ServerWidgetButtonPressed(AActor* WidgetOwner, int ButtonIndex);

private:
	UFUNCTION()
		bool TraceForInteraction(AActor*& HitActor);

	DECLARE_DELEGATE_OneParam(FSingleBoolDelegate, bool);

	//Input axis callback functions
	UFUNCTION()
		void MoveForwardCallback(float Value);

	UFUNCTION()
		void MoveRightCallback(float Value);

	UFUNCTION()
		void LookUpCallback(float Value);

	UFUNCTION()
		void TurnRightCallback(float Value);

	//Action mapping callback functions
	UFUNCTION()
		void InteractCallback(bool IsPressed);

	UFUNCTION()
		void JumpCallback(bool IsPressed);

	UFUNCTION()
		void PushToTalkCallback(bool IsPressed);

	UFUNCTION()
		void SwitchPerspectiveCallback();

	UFUNCTION()
		void ChangeSpeakDistanceCallback();
	
	UFUNCTION(Server, Reliable)
		void ServerOnPerspectiveSwitched(bool IsFirstPerson);

	UFUNCTION()
		void SprintCallback(bool IsPressed);

	UFUNCTION(Server, Reliable)
		void ServerChangeSpeakDistance(int Index);

	UFUNCTION(Server, Reliable)
		void ServerSetIsRunning(bool isRunning);

	//Callback Functions
	UFUNCTION()
		void OnPushToTalkChangedCallback(bool Value);


//ReadyPlayerMe functions
private:
	UPROPERTY()
		bool bWasPlayerStateAssigned = false;

	UPROPERTY(ReplicatedUsing = OnCharacterChanged)
		FString ReadyPlayerMeUrl;

public:	

	/**
	* Called from Playerstate when the PawnChanged delegate fires and the new Pawn is this Character.
	* 
	* @param NewPlayerState the Playerstate which was assigned to this character
	*/
	UFUNCTION()
		void OnPlayerStateAssigned(ABetaversePlayerState* NewPlayerState);

	/**
	*	Loads the given glTF file (ReadyPlayerMe character)
	*	Used to call the ServerSetReadyPlayerMe function for replication
	*
	* @param URL glTF file location
	*/
	UFUNCTION(BlueprintCallable)
		void SetNewReadyPlayerMe(const FString& URL);

private:
	UFUNCTION(Server, Reliable)
		void ServerSetNewReadyPlayerMe(const FString& URL);

	//Character control functions
	UFUNCTION(BlueprintCallable)
		void SetDefaultCharacter();

	/**
	*  Replicate Notify function when ReadyPlayerMeUrl changes.
	*  Loads the new character URL acordingly
	*/
	UFUNCTION()
		void OnCharacterChanged();
	
	/**
	*	Callback function when ready player me character is changed. Saves the new mesh inside the playerstate to preserve it in case of a servertravel
	*/
	UFUNCTION()
		void OnAnimInitilizedCallback();

	UFUNCTION()
		void OnReadyPlayerMeSetupFailed(const FString& ErrorMessage);

	UFUNCTION(BlueprintCallable, Category = "ReadyPlayerMe")
		FString GetReadyPlayerMeUrl() { return ReadyPlayerMeUrl; };
};
