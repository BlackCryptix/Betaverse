// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Whiteboard.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Actors/WhiteboardToolButton.h"
#include "Components/WidgetComponent.h"
#include "Character/CharacterMaster.h"
#include "Camera/CameraComponent.h"

#include <Runtime\Engine\Public\Net\UnrealNetwork.h>
#include <Runtime\Engine\Classes\Engine\Engine.h>

// Sets default values
AWhiteboard::AWhiteboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Board = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralBoardComp"));
	SetRootComponent(Board);


	ToolWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ToolWidget"));
	ToolWidget->SetupAttachment(Board);

}

// Called when the game starts or when spawned
void AWhiteboard::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		SetActorTickEnabled(false);
	}
}

void AWhiteboard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BoardSize = FVector2D(FMath::Min(BoardSize.X, 512.0f), FMath::Min(BoardSize.Y, 512.0f));
	ToolWidget->SetDrawSize(2.0f * BoardSize);


	TextureSize = FIntVector2(FMath::Min(4.0f * BoardSize.X, 2048.0f), FMath::Min(4.0f * BoardSize.Y, 2048.0f));
	DrawTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, TextureSize.X, TextureSize.Y, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	float markerScale = FMath::Max(TextureSize.X, TextureSize.Y) / 200.0f;

	BoardMaterialDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, BoardMaterial);
	BoardMaterialDynamic->SetTextureParameterValue(FName("DrawTexture"), DrawTexture);

	MarkerMaterialDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, MarkerMaterial);
	MarkerMaterialDynamic->SetScalarParameterValue(FName("URatio"), FMath::Max(BoardSize.X / BoardSize.Y, 1.0f) * markerScale);
	MarkerMaterialDynamic->SetScalarParameterValue(FName("VRatio"), FMath::Max(BoardSize.Y / BoardSize.X, 1.0f) * markerScale);


	//white board vertices
	TArray<int> Triangles;
	TArray<int> TrianglesBack;
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UVs;

	Vertices.Add(FVector(BoardSize.X, 0.0f, BoardSize.Y));
	Vertices.Add(FVector(-BoardSize.X, 0.0f, BoardSize.Y));
	Vertices.Add(FVector(-BoardSize.X, 0.0f, -BoardSize.Y));
	Vertices.Add(FVector(BoardSize.X, 0.0f, -BoardSize.Y));

	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);
	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(3);

	TrianglesBack.Add(0);
	TrianglesBack.Add(2);
	TrianglesBack.Add(1);
	TrianglesBack.Add(0);
	TrianglesBack.Add(3);
	TrianglesBack.Add(2);

	UVs.Add(FVector2D(0.0f, 0.0f));
	UVs.Add(FVector2D(1.0f, 0.0f));
	UVs.Add(FVector2D(1.0f, 1.0f));
	UVs.Add(FVector2D(0.0f, 1.0f));

	//Create procedural whiteboard
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);
	Board->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, true);
	Board->SetMaterial(0, BoardMaterialDynamic);

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, TrianglesBack, UVs, Normals, Tangents);
	Board->CreateMeshSection_LinearColor(1, Vertices, TrianglesBack, Normals, UVs, TArray<FLinearColor>(), Tangents, false);
	Board->SetMaterial(1, BoardBackMaterial);
}

// Called every frame
void AWhiteboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		TArray<ACharacterMaster*> characters;
		LastHitMap.GetKeys(characters);
		for (ACharacterMaster* character : characters)
		{
			FVector2D uvHit;
			if (TraceToWhitboard(character, uvHit))
			{
				if (FVector2D* lastUVHit = LastHitMap.Find(character))
				{
					if (!lastUVHit->Equals(uvHit, 0.01f))
					{
						MulticastDraw(character, uvHit);
					}
				}
			}
			else
			{
				MulticastClear(character);
			}
		}
	}
}

void AWhiteboard::DrawOnWhiteboard(FVector2D DrawLocation)
{
	MarkerMaterialDynamic->SetVectorParameterValue(FName("DrawLocation"), FVector4(DrawLocation.X, DrawLocation.Y, 0.0f, 0.0f));
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, DrawTexture, MarkerMaterialDynamic);
}

void AWhiteboard::OnColorReplicated()
{
	MarkerMaterialDynamic->SetVectorParameterValue(FName("Color"), MarkerColor);
}

void AWhiteboard::OnStrengthReplicated()
{
	MarkerMaterialDynamic->SetScalarParameterValue(FName("DrawStrength"), MarkerStrength);
}

void AWhiteboard::Interact_Implementation(ACharacterMaster* Character)
{
	Character->ServerInteract(this);
}

void AWhiteboard::InteractFromServer_Implementation(ACharacterMaster* Character)
{
	FVector2D hitUV;
	TraceToWhitboard(Character, hitUV);
	MulticastDraw(Character, hitUV);
}

void AWhiteboard::StopInteract_Implementation(ACharacterMaster* Character)
{
	Character->ServerStopInteract(this);
}

void AWhiteboard::StopInteractFromServer_Implementation(ACharacterMaster* Character)
{
	MulticastClear(Character);
}


void AWhiteboard::MulticastDraw_Implementation(ACharacterMaster* Character, const FVector2D& UV)
{
	if (FVector2D* lastHit = LastHitMap.Find(Character))
	{
		if (!lastHit->IsNearlyZero(0.01f) && !UV.IsNearlyZero(0.01f))
		{
			for (int i = 0; i < 20; i++)
			{
				DrawOnWhiteboard(*lastHit + i / 20.0f * (UV - *lastHit));
			}
		}
	}
	LastHitMap.Add(Character, UV);
}

void AWhiteboard::MulticastClear_Implementation(ACharacterMaster* Character)
{
	LastHitMap.Remove(Character);
}

bool AWhiteboard::TraceToWhitboard(ACharacterMaster* Character, FVector2D& UV)
{
	float offset = Character->GetInteractTraceOffset();
	FVector traceStart = Character->GetActiveCamera()->GetComponentLocation();
	FVector traceEnd = traceStart + Character->GetControlRotation().Vector() * (150.0f + offset);

	static const FName LineTraceSingleName(TEXT("InteractionLineTraceSingle"));
	FCollisionQueryParams CollisionParams;

	CollisionParams.bTraceComplex = false;
	CollisionParams.bReturnFaceIndex = true;

	//Debug Trace
	//const FName TraceTag("WhiteboardTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	//CollisionParams.TraceTag = TraceTag;
	
	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_GameTraceChannel1, CollisionParams))
	{
		if (this == hitResult.GetActor())
		{
			return UGameplayStatics::FindCollisionUV(hitResult, 0, UV);
		}
	}

	return false;
}

void AWhiteboard::OnSizeReplicated()
{
	MarkerMaterialDynamic->SetScalarParameterValue(FName("DrawSize"), MarkerSize);
}

void AWhiteboard::SetDrawSize(float Value)
{
	MarkerSize = Value;
	OnSizeReplicated();
}

void AWhiteboard::SetDrawStrength(float Value)
{
	MarkerStrength = Value;
	OnStrengthReplicated();
}

void AWhiteboard::SetDrawColor(const FLinearColor& Value)
{
	MarkerColor = Value;
	OnColorReplicated();
}

void AWhiteboard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWhiteboard, BoardSize);
	DOREPLIFETIME(AWhiteboard, MarkerColor);
	DOREPLIFETIME(AWhiteboard, MarkerSize);
	DOREPLIFETIME(AWhiteboard, MarkerStrength);
}

