// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHome/PlotLandscape.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APlotLandscape::APlotLandscape()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	LandscapeMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LandscapeMesh"));

}

// Called when the game starts or when spawned
void APlotLandscape::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlotLandscape::BuildLandscape(const FVector2D& PlotSize)
{
	LandscapeSize = PlotSize;

	//Plot vertices
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UVs;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;

	const float uvScale = VertexGap / 100.0f;
	const int xSize = LandscapeSize.X / uvScale;
	const int ySize = LandscapeSize.Y / uvScale;

	const int RealQuadLength = xSize + 1;

	//Generate Plot with UVs and Triangles
	for (int y = 0; y <= ySize; y++)
	{
		for (int x = 0; x <= xSize; x++)
		{
			UVs.Add(FVector2D(x, y) * uvScale);
			Vertices.Add(FVector((x * VertexGap), (y * VertexGap), 0.0f));

			if ((x < xSize) && (y < ySize))
			{
				Triangles.Add(RealQuadLength * y + x);
				Triangles.Add(RealQuadLength * (y + 1) + x);
				Triangles.Add(RealQuadLength * y + x + 1);

				Triangles.Add(RealQuadLength * y + x + 1);
				Triangles.Add(RealQuadLength * (y + 1) + x);
				Triangles.Add(RealQuadLength * (y + 1) + x + 1);
			}
		}
	}


	//Wall info
	TArray<FVector> verticesWall;
	TArray<int> trianglesWall;
	TArray<FVector2D> uvsWall;
	TArray<FVector> normalsWall;
	TArray<FProcMeshTangent> tangentsWall;

	//Create array with plot dimensions to drawn the wall along
	TArray<FVector> PlotShape;
	PlotShape.Add(FVector(xSize, ySize, 0.0f) * FVector(0.0f, 0.0f, 0.0f) * VertexGap);
	PlotShape.Add(FVector(xSize, ySize, 0.0f) * FVector(0.0f, 1.0f, 0.0f) * VertexGap);
	PlotShape.Add(FVector(xSize, ySize, 0.0f) * FVector(1.0f, 1.0f, 0.0f) * VertexGap);
	PlotShape.Add(FVector(xSize, ySize, 0.0f) * FVector(1.0f, 0.0f, 0.0f) * VertexGap);

	float currentPlotLength = 0.0f;

	//callculate wall vertices along plot
	for (int i = 0; i < PlotShape.Num(); i++)
	{
		FVector currentPoint = PlotShape[i];
		FVector lastPoint = PlotShape[(i - 1) < 0 ? PlotShape.Num() - 1 : i - 1];
		FVector nextPoint = PlotShape[(i + 1) % PlotShape.Num()];

		//Generate Walls
		FVector startPos = PlotShape[i];
		FVector endPos = PlotShape[(i + 1) % PlotShape.Num()];
		FVector direction = endPos - startPos;
		float edgeLength = direction.Size();

		FVector roofDirection = (currentPoint - lastPoint).GetUnsafeNormal2D() - (currentPoint - nextPoint).GetUnsafeNormal2D();
		roofDirection = FVector::CrossProduct(roofDirection, FVector(0, 0, 1)).GetUnsafeNormal2D();

		for (int j = 0; j < 4; j++)
		{
			trianglesWall.Add(i * 5 + j);
			trianglesWall.Add((i + 1) * 5 + j);
			trianglesWall.Add(i * 5 + 1 + j);

			trianglesWall.Add(i * 5 + 1 + j);
			trianglesWall.Add((i + 1) * 5 + j);
			trianglesWall.Add((i + 1) * 5 + 1 + j);
		}

		//calculate roofline
		verticesWall.Add(currentPoint);
		verticesWall.Add(currentPoint + FVector(0, 0, WallHeight));
		verticesWall.Add(currentPoint + (WallThickness * roofDirection + FVector(0, 0, WallHeight)));
		verticesWall.Add(currentPoint + (WallThickness * roofDirection + FVector(0, 0, -1)));
		verticesWall.Add(currentPoint);

		uvsWall.Add(FVector2D(currentPlotLength, 0) / 100.0f);
		uvsWall.Add(FVector2D(currentPlotLength, WallHeight) / 100.0f);
		uvsWall.Add(FVector2D(currentPlotLength, WallHeight + WallThickness) / 100.0f);
		uvsWall.Add(FVector2D(currentPlotLength, 2* WallHeight + WallThickness) / 100.0f);
		uvsWall.Add(FVector2D(currentPlotLength, 2 * WallHeight + 2 * WallThickness) / 100.0f);

		currentPlotLength += FMath::RoundToFloat(edgeLength);
	}

	//add first wall segment again to close the wall at the start
	FVector newV = verticesWall[0];
	verticesWall.Add(newV);
	newV = verticesWall[1];
	verticesWall.Add(newV);
	newV = verticesWall[2];
	verticesWall.Add(newV);
	newV = verticesWall[3];
	verticesWall.Add(newV);
	newV = verticesWall[4];
	verticesWall.Add(newV);

	uvsWall.Add(FVector2D(currentPlotLength, 0) / 100.0f);
	uvsWall.Add(FVector2D(currentPlotLength, WallHeight) / 100.0f);
	uvsWall.Add(FVector2D(currentPlotLength, WallHeight + WallThickness) * WallUVScale);
	uvsWall.Add(FVector2D(currentPlotLength, 2 * WallHeight + WallThickness) * WallUVScale);
	uvsWall.Add(FVector2D(currentPlotLength, 2 * WallHeight + 2 * WallThickness) * WallUVScale);



	//Create Wall and plot mesh section
	//Plot (Ground)
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);
	LandscapeMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, true);
	LandscapeMesh->SetMaterial(0, LandscapeMaterial);
	//Wall
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(verticesWall, trianglesWall, uvsWall, normalsWall, tangentsWall);
	LandscapeMesh->CreateMeshSection_LinearColor(1, verticesWall, trianglesWall, normalsWall, uvsWall, TArray<FLinearColor>(), tangentsWall, true);
	LandscapeMesh->SetMaterial(1, WallMaterial);

}


