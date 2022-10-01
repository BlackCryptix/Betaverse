// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact.h"
#include "Whiteboard.generated.h"

UCLASS()
class BETAVERSE_API AWhiteboard : public AActor, public IInteract, public IIndexButton
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWhiteboard();

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Whiteboard")
		class UProceduralMeshComponent* Board;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Whiteboard")
		class UWidgetComponent* ToolWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInterface* BoardBackMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInterface* BoardMaterial;
	UPROPERTY()
		UMaterialInstanceDynamic* BoardMaterialDynamic;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInterface* MarkerMaterial;
	UPROPERTY()
		UMaterialInstanceDynamic* MarkerMaterialDynamic;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
		FVector2D BoardSize = FVector2D(200.0f, 100.0f);

	FIntVector2 TextureSize;

	UPROPERTY()
		UTextureRenderTarget2D* DrawTexture;

private:
	UPROPERTY(ReplicatedUsing = OnColorReplicated)
		FLinearColor MarkerColor = FLinearColor::Black;

	UPROPERTY(ReplicatedUsing = OnSizeReplicated)
		float MarkerSize = 0.01f;

	UPROPERTY(ReplicatedUsing = OnStrengthReplicated)
		float MarkerStrength = 1.0f;

	UPROPERTY()
		bool bLastHitValid;

	UPROPERTY()
		TMap<ACharacterMaster*, FVector2D> LastHitMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Whitboard")
		void DrawOnWhiteboard(FVector2D DrawLocation);

	UFUNCTION(BlueprintCallable, Category = "Whitboard")
		void SetDrawSize(float Value);

	UFUNCTION(BlueprintCallable, Category = "Whitboard")
		void SetDrawStrength(float Value);

	UFUNCTION(BlueprintCallable, Category = "Whitboard")
		void SetDrawColor(const FLinearColor& Value);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Whitboard")
		float GetMarkerSize() { return MarkerSize; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Whitboard")
		float GetMarkerStrength() { return MarkerStrength; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Whitboard")
		FLinearColor GetMarkerColor() { return MarkerColor; };

public:
	//Interaction Interface
		void Interact(ACharacterMaster* Character);
	virtual void Interact_Implementation(ACharacterMaster* Character) override;
	
		void InteractFromServer(ACharacterMaster* Character);
	virtual void InteractFromServer_Implementation(ACharacterMaster* Character) override;

		void StopInteract(ACharacterMaster* Character);
	virtual void StopInteract_Implementation(ACharacterMaster* Character) override;

		void StopInteractFromServer(ACharacterMaster* Character);
	virtual void StopInteractFromServer_Implementation(ACharacterMaster* Character) override;

private:
	UFUNCTION()
		bool TraceToWhitboard(ACharacterMaster* Character, FVector2D& UV);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastDraw(ACharacterMaster* Character, const FVector2D& UV);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastClear(ACharacterMaster* Character);

	UFUNCTION()
		void OnColorReplicated();

	UFUNCTION()
		void OnStrengthReplicated();

	UFUNCTION()
		void OnSizeReplicated();
};
