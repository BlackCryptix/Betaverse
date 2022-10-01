// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PickUpObject.h"
#include "Components/CapsuleComponent.h"
//#include "Components/SkeletalMeshComponent.h"
#include "Character/CharacterMaster.h"

// Sets default values
APickUpObject::APickUpObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetSimulatePhysics(true);
	SetRootComponent(MeshComp);

}

// Called when the game starts or when spawned
void APickUpObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickUpObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUpObject::Interact_Implementation(ACharacterMaster* Character)
{
	Character->ServerInteract(this);
}

void APickUpObject::InteractFromServer_Implementation(ACharacterMaster* Character)
{
	MulticastInteraction(Character);
}

void APickUpObject::Drop()
{
	if (AActor* owner = GetOwner())
	{
		MeshComp->IgnoreActorWhenMoving(owner, false);
		Cast<ACharacterMaster>(owner)->GetMesh()->IgnoreActorWhenMoving(this, false);
	}
	SetOwner(nullptr);
	AttachToComponent(nullptr, FAttachmentTransformRules::KeepWorldTransform);
	MeshComp->SetSimulatePhysics(true);
}

void APickUpObject::Release()
{
	if (ACharacterMaster* owner = Cast<ACharacterMaster>(GetOwner()))
	{
		MeshComp->SetSimulatePhysics(true);
		MeshComp->IgnoreActorWhenMoving(owner, false);
		owner->GetMesh()->IgnoreActorWhenMoving(this, false);

		if (bThrowOnRelease)
		{
			FVector throwDirection = owner->GetControlRotation().Vector();
			SetActorLocation(GetActorLocation() + throwDirection * 50.0f);
			MeshComp->AddImpulse(throwDirection * 10000.0f);
		}
		else
		{
			
		}
	}
	SetOwner(nullptr);
	AttachToComponent(nullptr, FAttachmentTransformRules::KeepWorldTransform);

}

void APickUpObject::MulticastInteraction_Implementation(ACharacterMaster* Character)
{
	if (AActor* owner = GetOwner())
	{
		MeshComp->IgnoreActorWhenMoving(owner, false);
		Cast<ACharacterMaster>(owner)->GetMesh()->IgnoreActorWhenMoving(this, false);
	}

	MeshComp->SetSimulatePhysics(false);
	Character->GetCapsuleComponent()->IgnoreActorWhenMoving(this, true);
	MeshComp->IgnoreActorWhenMoving(Character, true);
	AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHand"));

	SetOwner(Character);
	Character->SetCarryItem(this);
}

