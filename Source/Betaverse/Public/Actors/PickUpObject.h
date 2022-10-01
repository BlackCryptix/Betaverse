// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact.h"
#include "PickUpObject.generated.h"

UCLASS()
class BETAVERSE_API APickUpObject : public AActor, public IInteract
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpObject();

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "PickUpObject")
		UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bThrowOnRelease = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

		void Interact(ACharacterMaster* Character);
	virtual void Interact_Implementation(ACharacterMaster* Character) override;

		void InteractFromServer(ACharacterMaster* Character);
	virtual void InteractFromServer_Implementation(ACharacterMaster* Character) override;

	UFUNCTION()
		void Drop();

	UFUNCTION()
		void Release();

private:
	UFUNCTION(NetMulticast, Reliable)
		void MulticastInteraction(ACharacterMaster* Character);

};
