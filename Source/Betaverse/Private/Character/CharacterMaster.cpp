// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterMaster.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "ReadyPlayerMeActorComponent.h"

#include "BetaverseInstance.h"
#include "BetaversePlayerState.h"
#include "SaveGames/SettingsSaveGame.h"
#include "Interfaces/PlayerNameWidgetInterface.h"
#include "Interfaces/Interact.h"
#include "Actors/PickUpObject.h"

#include <Runtime\Engine\Public\Net\UnrealNetwork.h>
#include <Runtime\Engine\Classes\Engine\Engine.h>

// Sets default values
ACharacterMaster::ACharacterMaster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup first person camera
	FPCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPCameraArm"));
	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));

	FPCameraArm->SetupAttachment(GetMesh(), FName("head"));
	FPCamera->SetupAttachment(FPCameraArm);

	FPCameraArm->AddLocalRotation(FRotator(0.0f, -90.0f, 0.0f));
	FPCameraArm->TargetArmLength = 11.0f;

	FPCamera->bUsePawnControlRotation = true;
	FPCamera->AddLocalRotation(FRotator(0.0f, 180.0f, 0.0f));
	FPCamera->FieldOfView = 120.0f;

	//Setup Third Person Camera
	TPCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPCameraArmComp"));
	TPCameraArmRight = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPCameraArmRight"));
	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));

	TPCameraArm->SetupAttachment(RootComponent);
	TPCameraArmRight->SetupAttachment(TPCameraArm);
	TPCamera->SetupAttachment(TPCameraArmRight);

	TPCameraArm->AddRelativeLocation(FVector(0, 0, 60));
	TPCameraArm->bDoCollisionTest = false;
	TPCameraArm->bUsePawnControlRotation = true;
	TPCameraArm->TargetArmLength = 300.0f;
	TPCameraArm->ProbeSize = 1.0f;
	TPCameraArm->bEnableCameraLag = true;
	TPCameraArm->CameraLagMaxDistance = 30.0f;

	TPCameraArmRight->AddLocalRotation(FRotator(0.0f, -90.0f, 0.0f));
	TPCameraArmRight->bDoCollisionTest = false;
	TPCameraArmRight->TargetArmLength = 50.0f;

	TPCamera->AddLocalRotation(FRotator(0.0f, 90.0f, 0.0f));

	//Set Movementvars
	GetCharacterMovement()->bOrientRotationToMovement = !bFirstPerson;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	ReadyPlayerMeComp = CreateDefaultSubobject<UReadyPlayerMeActorComponent>(TEXT("ReadyPlayerMeComponent"));
	VOIPRangeIndicator = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VOIPRangeIndicatorComponent"));

	VOIPRangeIndicator->SetupAttachment(GetMesh());
	VOIPRangeIndicator->bAutoActivate = false;

	PlayerNameComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameComponent"));
	PlayerNameComp->SetupAttachment(GetMesh(), FName("Socket_PlayerName"));
	PlayerNameComp->SetWidgetSpace(EWidgetSpace::Screen);
	PlayerNameComp->SetDrawSize(FVector2D(100.0f, 100.0f));
}

void ACharacterMaster::SetCarryItem(APickUpObject* Item)
{
	if (IsValid(CarryItem))
	{
		CarryItem->Drop();
	}
	CarryItem = Item;
}

// Called when the game starts or when spawned
void ACharacterMaster::BeginPlay()
{
	Super::BeginPlay();

	//Bind Push to talk setting and get saved readyplayerme char
	if (IsLocallyControlled())
	{
		BetaverseInstance = GetGameInstance<UBetaverseInstance>();
		SetNewReadyPlayerMe(BetaverseInstance->Settings->SelectedCharacterUrl);

		BetaverseInstance->Settings->OnUsePushToTalkChanged.AddUniqueDynamic(this, &ACharacterMaster::OnPushToTalkChangedCallback);

		BetaverseInstance->SetLocalPlayerCharacter(this);
	}

	GetMesh()->OnAnimInitialized.AddUniqueDynamic(this, &ACharacterMaster::OnAnimInitilizedCallback);

	//Setup Voice
	if (!SpeakDistances.IsEmpty())
	{
		if (!SpeakDistances.IsValidIndex(SpeakDistanceIndex))
		{
			SpeakDistanceIndex = 0;
		}

		if (IsLocallyControlled())
		{
			VOIPRangeIndicator->SetNiagaraVariableFloat(FString("Size"), SpeakDistances[SpeakDistanceIndex]->Attenuation.FalloffDistance * 2.0f);
			VOIPRangeIndicator->Activate();
		}
	}

	//Assign the this player char to its widget in order to set the corret player name
	UUserWidget* PlayerNameWidget = PlayerNameComp->GetWidget();
	if (PlayerNameWidget)
	{
		if (PlayerNameWidget->Implements<UPlayerNameWidgetInterface>() || Cast<IPlayerNameWidgetInterface>(PlayerNameWidget))
		{
			IPlayerNameWidgetInterface::Execute_SetPlayerCharacter(PlayerNameWidget, this);
		}
	}
}

// Called every frame
void ACharacterMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		AActor* hitInteractionActor;
		if (TraceForInteraction(hitInteractionActor))
		{
			if (hitInteractionActor != InteractionActor)
			{
				InteractionActor = hitInteractionActor;
				OnCanInteractChanged.Broadcast(true);
			}
		}
		else if(IsValid(InteractionActor))
		{
			InteractionActor = nullptr;
			OnCanInteractChanged.Broadcast(false);
		}
	}
	else
	{
		BetaverseInstance = GetGameInstance<UBetaverseInstance>();
		if(BetaverseInstance)
			BetaverseInstance->UpdatePlayerVoicePosition(this);
	}
}

// Called to bind functionality to input
void ACharacterMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &ACharacterMaster::MoveForwardCallback);
	InputComponent->BindAxis("MoveRight", this, &ACharacterMaster::MoveRightCallback);
	InputComponent->BindAxis("LookUp", this, &ACharacterMaster::LookUpCallback);
	InputComponent->BindAxis("TurnRight", this, &ACharacterMaster::TurnRightCallback);

	InputComponent->BindAction<FSingleBoolDelegate>("Jump", EInputEvent::IE_Pressed, this, &ACharacterMaster::JumpCallback, true);
	InputComponent->BindAction<FSingleBoolDelegate>("Jump", EInputEvent::IE_Released, this, &ACharacterMaster::JumpCallback, false);
	InputComponent->BindAction<FSingleBoolDelegate>("Interact", EInputEvent::IE_Pressed, this, &ACharacterMaster::InteractCallback, true);
	InputComponent->BindAction<FSingleBoolDelegate>("Interact", EInputEvent::IE_Released, this, &ACharacterMaster::InteractCallback, false);
	InputComponent->BindAction<FSingleBoolDelegate>("PushToTalk", EInputEvent::IE_Pressed, this, &ACharacterMaster::PushToTalkCallback, true);
	InputComponent->BindAction<FSingleBoolDelegate>("PushToTalk", EInputEvent::IE_Released, this, &ACharacterMaster::PushToTalkCallback, false);

	InputComponent->BindAction<FSingleBoolDelegate>("Sprint", EInputEvent::IE_Pressed, this, &ACharacterMaster::SprintCallback, true);
	InputComponent->BindAction<FSingleBoolDelegate>("Sprint", EInputEvent::IE_Released, this, &ACharacterMaster::SprintCallback, false);

	InputComponent->BindAction("ChangeSpeakDistance", EInputEvent::IE_Released, this, &ACharacterMaster::ChangeSpeakDistanceCallback);
	InputComponent->BindAction("SwitchPerspective", EInputEvent::IE_Released, this, &ACharacterMaster::SwitchPerspectiveCallback);

}

float ACharacterMaster::GetInteractTraceOffset()
{ 
	return bFirstPerson ? 0.0f : TPCameraArm->TargetArmLength; 
}

void ACharacterMaster::ServerInteract_Implementation(AActor* CallbackActor)
{
	IInteract::Execute_InteractFromServer(CallbackActor, this);
}

void ACharacterMaster::ServerStopInteract_Implementation(AActor* CallbackActor)
{
	IInteract::Execute_StopInteractFromServer(CallbackActor, this);
}

void ACharacterMaster::OnPlayerStateAssigned(ABetaversePlayerState* NewPlayerState)
{
	//loads ReadyPlayerMe character after server travel to prevent loading it form glTF file
	bWasPlayerStateAssigned = true;
	if (NewPlayerState->PersistantCharacterMesh)
	{
		GetMesh()->SetSkeletalMesh(NewPlayerState->PersistantCharacterMesh);
	}
	else if (!ReadyPlayerMeUrl.IsEmpty())
	{
		OnCharacterChanged();
	}
}

void ACharacterMaster::ServerWidgetButtonPressed_Implementation(AActor* WidgetOwner, int ButtonIndex)
{
	if (Cast<IIndexButton>(WidgetOwner) || WidgetOwner->Implements<UIndexButton>())
	{
		IIndexButton::Execute_PressButtonAtIndex(WidgetOwner, ButtonIndex);
	}
}

bool ACharacterMaster::TraceForInteraction(AActor*& HitActor)
{	
	const FVector cameraLocation = GetActiveCamera()->GetComponentLocation();
	const FVector cameraForward = GetActiveCamera()->GetForwardVector();

	float traceStartOffset = GetInteractTraceOffset();

	const FVector start = cameraLocation + cameraForward * traceStartOffset;
	const FVector end = cameraLocation + cameraForward * (traceStartOffset + 200.0f);

	static const FName LineTraceSingleName(TEXT("InteractionLineTraceSingle"));
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(this);
	CollisionParams.bTraceComplex = false;
	CollisionParams.bReturnFaceIndex = true;

	//Debug Trace
	//const FName TraceTag("MyTrace");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	//CollisionParams.TraceTag = TraceTag;

	if (GetWorld()->LineTraceSingleByChannel(CurrentInteractionHit, start, end, ECC_Visibility, CollisionParams))
	{
		if (Cast<IInteract>(CurrentInteractionHit.GetActor()) || CurrentInteractionHit.GetActor()->Implements<UInteract>())
		{
			//Set interaction object
			HitActor = CurrentInteractionHit.GetActor();
			return true;
		}
	}
	return false;
}

void ACharacterMaster::MoveForwardCallback(float Value)
{
	AddMovementInput(FRotator(0, GetControlRotation().Yaw, 0).Vector(), Value, false);
}

void ACharacterMaster::MoveRightCallback(float Value)
{
	AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value, false);
}

void ACharacterMaster::LookUpCallback(float Value)
{
	AddControllerPitchInput(Value);
}

void ACharacterMaster::TurnRightCallback(float Value)
{
	AddControllerYawInput(Value);
}

void ACharacterMaster::InteractCallback(bool IsPressed)
{
	if (IsPressed && IsValid(CarryItem))
	{
		CarryItem->Release();
		CarryItem = nullptr;
	}
	if (IsValid(InteractionActor))
	{
		if (IsPressed)
		{
			IInteract::Execute_Interact(InteractionActor, this);
		}
		else
		{
			IInteract::Execute_StopInteract(InteractionActor, this);
		}
	}
}

void ACharacterMaster::JumpCallback(bool IsPressed)
{
	if (IsPressed)
	{
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ACharacterMaster::PushToTalkCallback(bool IsPressed)
{
	UBetaverseInstance* gameInstance = GetGameInstance<UBetaverseInstance>();
	if (gameInstance->Settings->bUsePushToTalk)
	{
		gameInstance->K2_Mute(GetPlayerState()->GetUniqueId(), !IsPressed);
	}
}


void ACharacterMaster::SwitchPerspectiveCallback()
{
	//adjust controls to fit the perspective
	bFirstPerson = !bFirstPerson;
	TPCamera->SetActive(!bFirstPerson, false);
	FPCamera->SetActive(bFirstPerson, false);

	bUseControllerRotationYaw = bFirstPerson;
	GetCharacterMovement()->bOrientRotationToMovement = !bFirstPerson;
	ServerOnPerspectiveSwitched(bFirstPerson);
}

void ACharacterMaster::ServerOnPerspectiveSwitched_Implementation(bool IsFirstPerson)
{
	bFirstPerson = IsFirstPerson;
	bUseControllerRotationYaw = bFirstPerson;
	GetCharacterMovement()->bOrientRotationToMovement = !bFirstPerson;
}

void ACharacterMaster::SprintCallback(bool IsPressed)
{
	bIsRuning = IsPressed;
	ServerSetIsRunning(bIsRuning);
	if (bIsRuning)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	}
}

#pragma region Change and replicate VOIP talk range

void ACharacterMaster::ChangeSpeakDistanceCallback()
{
	if (!SpeakDistances.IsEmpty())
	{
		SpeakDistanceIndex++;
		if (!SpeakDistances.IsValidIndex(SpeakDistanceIndex))
		{
			SpeakDistanceIndex = 0;
		}

		if (!HasAuthority())
		{
			ServerChangeSpeakDistance(SpeakDistanceIndex);
		}

		VOIPRangeIndicator->SetNiagaraVariableFloat(FString("Size"), SpeakDistances[SpeakDistanceIndex]->Attenuation.FalloffDistance * 2.0f);
		VOIPRangeIndicator->Activate(true);
	}
}

void ACharacterMaster::ServerSetIsRunning_Implementation(bool isRunning)
{
	bIsRuning = isRunning;
	if (bIsRuning)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	}
}

void ACharacterMaster::ServerChangeSpeakDistance_Implementation(int Index)
{
	SpeakDistanceIndex = Index;
}

void ACharacterMaster::OnPushToTalkChangedCallback(bool Value)
{
	UBetaverseInstance* gameInstance = GetGameInstance<UBetaverseInstance>();
	gameInstance->K2_Mute(GetPlayerState()->GetUniqueId(), Value);
}

#pragma endregion


void ACharacterMaster::SetNewReadyPlayerMe(const FString& URL)
{
	ReadyPlayerMeUrl = URL;
	OnCharacterChanged();
	if (!HasAuthority())
	{
		ServerSetNewReadyPlayerMe(URL);
	}
}

bool ACharacterMaster::GetUVHitResult(FVector2D& InteractionHit)
{	
	return IsValid(InteractionActor) && UGameplayStatics::FindCollisionUV(CurrentInteractionHit, 0, InteractionHit);
}

void ACharacterMaster::ServerSetNewReadyPlayerMe_Implementation(const FString& URL)
{
	ReadyPlayerMeUrl = URL;
	OnCharacterChanged();
}

void ACharacterMaster::OnCharacterChanged()
{
	if (bWasPlayerStateAssigned)
	{
		if (!ReadyPlayerMeUrl.IsEmpty())
		{
			FAvatarLoadFailed AvatarLoadFailedDelegate;
			AvatarLoadFailedDelegate.BindUFunction(this, FName("OnReadyPlayerMeSetupFailed"));
			ReadyPlayerMeComp->LoadNewAvatar(ReadyPlayerMeUrl, AvatarLoadFailedDelegate);
		}
		else
		{
			SetDefaultCharacter();
		}
	}
}

void ACharacterMaster::SetDefaultCharacter()
{
	GetMesh()->SetSkeletalMesh(DefaultMesh);
}

void ACharacterMaster::OnAnimInitilizedCallback()
{
	ABetaversePlayerState* playerState = GetPlayerState<ABetaversePlayerState>();
	if (IsValid(playerState))
	{
		playerState->PersistantCharacterMesh = GetMesh()->SkeletalMesh;
	}
}

void ACharacterMaster::OnReadyPlayerMeSetupFailed(const FString& ErrorMessage)
{
	SetDefaultCharacter();
}

void ACharacterMaster::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACharacterMaster, SpeakDistanceIndex, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ACharacterMaster, ReadyPlayerMeUrl, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ACharacterMaster, bIsRuning, COND_SkipOwner);
}
