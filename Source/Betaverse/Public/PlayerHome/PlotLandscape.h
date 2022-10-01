// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlotLandscape.generated.h"

UCLASS()
class BETAVERSE_API APlotLandscape : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlotLandscape();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D LandscapeSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float VertexGap = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WallHeight = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WallThickness = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WallUVScale = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInstance* LandscapeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInstance* WallMaterial;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Whiteboard")
		class UProceduralMeshComponent* LandscapeMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
		void BuildLandscape(const FVector2D& PlotSize);
};
