// Copyright (C) 2019-2020 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.

#pragma once

#include "Curves/CurveFloat.h"
#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "VFSpline.h"
#include "Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
//#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
//#include "Factories/VectorFieldStaticFactory.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "VFBound.generated.h"

UENUM()
enum class ESplineCombineMode : uint8
{
	/** Uses the addition value: a+b */
	Add = 0,
	/** Uses the average value: (a+b)/2 */
	Average = 1,
	/** Uses the minimum value: min(a,b) */
	Min = 2,
	/** Uses the product of the values: a*b */
	Multiply = 3,
	/** Uses the maximum value: max(a,b) */
	Max = 4
};

USTRUCT(BlueprintType)
struct FAdvancedSplineData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "Spline weight"))
	float SplineWeight = 1.f;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (DisplayName = "Additional metod"))
	ESplineCombineMode AdditionalMethod;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (ClampMin = "0.0", ClampMax = "90.0", DisplayName = "In angle"))
	float InAngle = 0.f;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (ClampMin = "0.0", ClampMax = "90.0", DisplayName = "Out angle"))
	float OutAngle = 0.f;
	
	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (DisplayName = "Twist curve"))
	UCurveFloat* TwistCurve;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (DisplayName = "Pitch vectors curve"))
		UCurveFloat* VectorsLerpCurve;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (DisplayName = "Speed curve"))
	UCurveFloat* SpeedCurve;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (ClampMin = "-1.0", ClampMax = "1.0", DisplayName = "Min speed random"))
		float MinSpeedRandom = 1.f;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (ClampMin = "-1.0", ClampMax = "1.0", DisplayName = "Max speed random"))
		float MaxSpeedRandom = 1.f;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (ClampMin = "0.0", ClampMax = "180.0", DisplayName = "Random in cone"))
		float RandomInConeAngle = 0.f;
};

USTRUCT(BlueprintType)
struct FSplineData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "VFCreator")
	class AVFSpline* VFSpline = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (DisplayName = "Falloff curve"))
	UCurveFloat* FallOffCurve; //FRuntimeFloatCurve FallOffCurve;

	UPROPERTY(EditInstanceOnly, Category = "VFCreator", meta = (DisplayName = "Advanced properties of spline"))
	FAdvancedSplineData AdvancedSplineProperty;
};

UCLASS(hidecategories = ("Rendering", "Input", "Actor", "Replication", "Collision", "LOD", "Cooking"))
class VFCREATOR_API AVFBound : public AActor
{
	//GENERATED_UCLASS_BODY()
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVFBound();

	/** Root component */
	UPROPERTY()
		UBillboardComponent* SpriteComponent;

	UPROPERTY()
		UBoxComponent* BoundBox;

	UPROPERTY()
		USphereComponent* ShowSphere;

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditInstanceOnly, Category = "Vector Field Creator", meta = (DisplayName = "File name"))
		FString FileName = "MyVectorField";

	UPROPERTY(EditInstanceOnly, Category = "Vector Field Bounds", meta = (DisplayName = "Show grid"))
		bool bShowGrid = false;

	/** Numbers of grid */
	UPROPERTY(EditInstanceOnly, Category = "Vector Field Bounds", meta = (DisplayName = "Resolution"))
		FVector Grid = FVector(8.f, 8.f, 8.f);

	UPROPERTY(VisibleInstanceOnly, Category = "Vector Field Bounds", meta = (DisplayName = "Voxel count"))
		int32 NumGrids = 512;

	UPROPERTY(EditInstanceOnly, Category = "Vector Field Bounds", meta = (EditCondition = "!bBoundaryMode", DisplayName = "Cell size"))
		FVector GridExtends = FVector(16.f, 16.f, 16.f);

	UPROPERTY(EditInstanceOnly, Category = "Vector Field Bounds", meta = (DisplayName = "Boundary mode"))
		bool bBoundaryMode = false;

	/** Bound */
	UPROPERTY(EditInstanceOnly, Category = "Vector Field Bounds", meta = (EditCondition = "bBoundaryMode", DisplayName = "Field size"))
		FVector Extends = FVector(128.f, 128.f, 128.f);

	UPROPERTY(EditInstanceOnly, Category = "Vector Field Bounds", meta = (DisplayName = "Pivot offset"))
		FVector PivotPointOffset = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, Category = "Visualisation", meta = (DisplayName = "Beacon mode"))
		bool bShowVectorsInSphere = false;
	/** Show Vectors In Sphere when You move sphere */
	UPROPERTY(EditInstanceOnly, Category = "Visualisation", meta = (MakeEditWidget = bShowVectorsInSphere, EditCondition = "bShowVectorsInSphere", DisplayName = "Beacon live update"))
		bool bUpdateVectorsInSphereWhenMove = false;

	UPROPERTY(EditInstanceOnly, Category = "Visualisation", meta = (MakeEditWidget = bShowVectorsInSphere, EditCondition = "bShowVectorsInSphere", DisplayName = "Beacon location"))
		FVector CenterShowSphere = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, Category = "Visualisation", meta = (EditCondition = "bShowVectorsInSphere", DisplayName = "Beacon radius"))
		float SphereRadius = 256.f;

	UPROPERTY(EditInstanceOnly, Category = "Visualisation", meta = (ClampMin = "0.0", DisplayName = "Velocity draw skip"))
		int32 SkipGridsNum = 0;

	UPROPERTY(EditInstanceOnly, Category = "Visualisation", meta = (ClampMin = "0.0", DisplayName = "Arrows scale"))
		float VectorSize = 1.f;

	UPROPERTY(EditInstanceOnly, Category = "Visualisation", meta = (ClampMin = "0.0", DisplayName = "Arrows length"))
		float VectorLengthScale = 1.f;

//	UPROPERTY(VisibleInstanceOnly, Category = "Visualisations")
		int32 NumArrows;
	
	UPROPERTY(EditInstanceOnly, Category = "Vector Field Splines", meta = (DisplayName = "Array of VFSplines"))
	TArray<FSplineData> VFSplinesArray;

	UPROPERTY(EditInstanceOnly, Category = "Vector Field Splines", meta = (DisplayName = "Normalize"))
	bool bUseNormalize = false;

//protected:

	TArray<FVector> Vectors;
	
	TArray<class UBoxComponent*> GridBoxes;

	TArray<UArrowComponent*> GridArrows;

	float MaxVectorLength = 0.f;

	bool HoldDraw;

//	bool Modify(bool bAlwaysMarkDirty/*=true*/) override;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void RemoveBoxes();

	void RemoveVectors();

	void GenerateGrid();

	void OnCalculateClicked();

	float GetDistanceAlongSpline(const USplineComponent* InSpline, const FVector& InLocation);

	void ShowVectors();

	void SaveToFile();
#endif
};