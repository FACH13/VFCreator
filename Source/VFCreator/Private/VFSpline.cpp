// Copyright (C) 2019-2024 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.

#include "VFSpline.h"
#include "VFCreator.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values

AVFSpline::AVFSpline(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(SplineComponent);

#if WITH_EDITORONLY_DATA
	SplineComponent->bShouldVisualizeScale = true;
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(79, 22, 159);
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SetupAttachment(RootComponent);
		ArrowComponent->bIsScreenSizeScaled = true;
	}
#endif // WITH_EDITORONLY_DATA
}

void AVFSpline::OnConstruction(const FTransform& Transform)
{
#if WITH_EDITORONLY_DATA
	FVector LocalLocation, LocalTangent;
	SplineComponent->GetLocationAndTangentAtSplinePoint(0, LocalLocation, LocalTangent, ESplineCoordinateSpace::World);
	ArrowComponent->SetWorldLocation(LocalLocation);
	ArrowComponent->SetWorldRotation(LocalTangent.ToOrientationRotator());
#endif // WITH_EDITORONLY_DATA
}
