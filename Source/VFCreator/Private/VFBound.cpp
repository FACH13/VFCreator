// Copyright (C) 2019-2020 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.

#include "VFBound.h"
#include "VFCreator.h"

// Sets default values
// AVFBound::AVFBound(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
AVFBound::AVFBound()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITOR
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	SetRootComponent(SpriteComponent);

	if (!IsRunningCommandlet() && (SpriteComponent != nullptr))
	{
		// Structure to hold one-time initialization
		
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> EffectsTextureObject;
			FName ID_Effects;
			FText NAME_Effects;
			FConstructorStatics()
				: EffectsTextureObject(TEXT("/Engine/EditorResources/S_VectorFieldVol"))
				, ID_Effects(TEXT("Effects"))
				, NAME_Effects(NSLOCTEXT("SpriteCategory", "Effects", "Effects"))
			{
			}
		};
		FConstructorStatics ConstructorStatics;

		SpriteComponent->Sprite = ConstructorStatics.EffectsTextureObject.Get();
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Effects;
		SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Effects;
		SpriteComponent->SetUsingAbsoluteScale(true);
		SpriteComponent->bReceivesDecals = false;
 	}
	BoundBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundBox"));
	if (BoundBox != nullptr)
	{
		BoundBox->SetupAttachment(RootComponent);
		BoundBox->ShapeColor = FColor::Red;
		BoundBox->InitBoxExtent(Extends);
		BoundBox->SetRelativeLocation(PivotPointOffset);
	}
	ShowSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Show Sphere"));
	ShowSphere->SetSphereRadius(SphereRadius);
	ShowSphere->SetupAttachment(RootComponent);
	ShowSphere->ShapeColor = FColor::Green;
	ShowSphere->SetVisibility(bShowVectorsInSphere);
#endif// WITH_EDITORONLY_DATA
}

// bool AVFBound::Modify(bool bAlwaysMarkDirty/*=true*/)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("ProcessEvent"));
// 	return Super::Modify(bAlwaysMarkDirty);
// }

#if WITH_EDITORONLY_DATA//WITH_EDITORONLY_DATA
void AVFBound::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Grid.X = FMath::Min(FMath::Max(FMath::RoundToZero(Grid.X), 1.f), 1024.f);
	Grid.Y = FMath::Min(FMath::Max(FMath::RoundToZero(Grid.Y), 1.f), 1024.f);
	Grid.Z = FMath::Min(FMath::Max(FMath::RoundToZero(Grid.Z), 1.f), 1024.f);
//	assert(Grid.X * Grid.Y * Grid.Z > MAX_int32);
	NumGrids = Grid.X * Grid.Y * Grid.Z;

	if (!bBoundaryMode)
		Extends = GridExtends * Grid;
	else
		GridExtends = Extends / Grid;

	BoundBox->InitBoxExtent(Extends);
	BoundBox->SetRelativeLocation(PivotPointOffset);

	//Get the name of the property that was changed  
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
//	UE_LOG(LogTemp, Warning, TEXT("Property %s"), *PropertyName.ToString());
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVFBound, bShowGrid) ||
		PropertyName != "X" ||
		PropertyName != "Y" ||
		PropertyName != "Z")
	{
		if (bShowGrid)
		{
			RemoveBoxes();
			GenerateGrid();
		}
		else
		{
			RemoveBoxes();
		}
	}
	else
	{
		if (Grid.X + Grid.Y + Grid.Z < 25.f && bShowGrid)
			GenerateGrid();
		else
			if (bShowGrid)
			{
				bShowGrid = false;
				RemoveBoxes();
			}
	}
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVFBound, SphereRadius))
		ShowSphere->SetSphereRadius(SphereRadius);
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVFBound, bShowVectorsInSphere))
		ShowSphere->SetVisibility(bShowVectorsInSphere);
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVFBound, bShowGrid) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AVFBound, CenterShowSphere))
	{
		ShowSphere->SetRelativeLocation(CenterShowSphere);
		if (bUpdateVectorsInSphereWhenMove)
			ShowVectors();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVFBound, bUpdateVectorsInSphereWhenMove))
		if (bShowVectorsInSphere)
			ShowVectors();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void AVFBound::RemoveBoxes()
{
	for (UBoxComponent* GridBox : GridBoxes)
		GridBox->DestroyComponent();

	GridBoxes.Empty();
	RegisterAllComponents();
}

void AVFBound::RemoveVectors()
{
	if (GridArrows.Num() > 0)
	{
		for (UArrowComponent* Arrow : GridArrows)
			Arrow->DestroyComponent();
		RegisterAllComponents();
		GridArrows.Empty();
	}
}

void AVFBound::GenerateGrid()
{
	for (int32 i = 0; i <= Grid.X + Grid.Y + Grid.Z - 3.f; i++)
	{
		if (!GridBoxes.IsValidIndex(i))
		{
			GridBoxes.Add(NewObject<UBoxComponent>(this));
			GridBoxes[i]->RegisterComponent();
			GridBoxes[i]->AttachToComponent(BoundBox, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true));
		}
		GridBoxes[i]->SetBoxExtent(Extends / Grid);

		FVector LocalOffset = Extends * 2.f / Grid;
		LocalOffset.X *= (i >= 0 && i < Grid.X) ? i : 0.f;
		LocalOffset.Y *= (i >= Grid.X && i < Grid.X + Grid.Y - 1.f) ? i - Grid.X + 1.f : 0.f;
		LocalOffset.Z *= (i >= Grid.X + Grid.Y - 1.f && i < Grid.X + Grid.Y + Grid.Z) ? i - Grid.X - Grid.Y + 2.f : 0.f;
		GridBoxes[i]->SetRelativeLocation(LocalOffset - Extends + Extends / Grid);
	}
}

void AVFBound::OnCalculateClicked()
{
	if (VFSplinesArray.Num() == 0 || Extends.X < 1.f || Extends.Y < 1.f || Extends.Z < 1.f)
	{
		if (VFSplinesArray.Num() == 0)
			UE_LOG(LogTemp, Error, TEXT("VFSpline not set"))
		else 
			UE_LOG(LogTemp, Error, TEXT("Wrong grid size"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Start Calculate"));

	RemoveVectors();
	Vectors.Empty();
	Vectors.AddZeroed(NumGrids);
	NumArrows = 0;

	for (FSplineData SplA : VFSplinesArray)
	{
		AVFSpline* Spl = SplA.VFSpline;

		if (Spl == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Fall Off Curve not set"))
			continue;
		}

		if (SplA.FallOffCurve == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Fall Off Curve not set"))
			continue;
		}

		bool bNeedAverage = false;

		for (int32 i = 0; i < NumGrids; i++)
		{
			FVector GridLocation = Extends * 2 / Grid;
			GridLocation.X *= FMath::RoundToZero(FMath::Fmod(i, Grid.X));
			GridLocation.Y *= FMath::RoundToZero(FMath::Fmod(i / Grid.X, Grid.Y));
			GridLocation.Z *= FMath::RoundToZero(FMath::Fmod(i / Grid.X / Grid.Y, Grid.Z));
			GridLocation = GridLocation - Extends + Extends / Grid + PivotPointOffset;
			GridLocation = GetActorRotation().RotateVector(GridLocation);
			GridLocation += GetActorLocation();

			float DistanceAlongSpline = GetDistanceAlongSpline(Spl->GetSpline(), GridLocation);			
			FVector SplineScale = Spl->GetSpline()->GetScaleAtDistanceAlongSpline(DistanceAlongSpline);
			FVector SplineDirection = Spl->GetSpline()->FindDirectionClosestToWorldLocation(GridLocation, ESplineCoordinateSpace::World);
			FVector LocalDelta = Spl->GetSpline()->FindLocationClosestToWorldLocation(GridLocation, ESplineCoordinateSpace::World) - GridLocation;

			if ((SplineDirection | LocalDelta.GetSafeNormal()) >= FMath::Cos((180.f - SplA.AdvancedSplineProperty.OutAngle) * PI / 180.f) &&
				(SplineDirection | LocalDelta.GetSafeNormal()) <= FMath::Cos(SplA.AdvancedSplineProperty.InAngle * PI / 180.f))
			{
				float GridOffset, FallOffmax = 0.f;

				SplA.FallOffCurve->GetTimeRange(GridOffset, FallOffmax);

				GridOffset = FMath::Square(LocalDelta | Spl->GetSpline()->GetRightVectorAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World) / SplineScale.Y);
				GridOffset += FMath::Square(LocalDelta | Spl->GetSpline()->GetUpVectorAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World) / SplineScale.Z);
				GridOffset = FMath::Sqrt(GridOffset);

				FVector LocalVector = FVector::ZeroVector;

				if (GridOffset <= FallOffmax)
				{
					float SpeedScale = 1.f;
					float VelocityLerp, TwistAngle = 0.f;

					if (SplA.AdvancedSplineProperty.VectorsLerpCurve != nullptr)
					{
						VelocityLerp = SplA.AdvancedSplineProperty.VectorsLerpCurve->GetFloatValue(GridOffset);
						LocalVector = SplineDirection.RotateAngleAxis(VelocityLerp, SplineDirection ^ LocalDelta.GetSafeNormal());
						LocalVector *= FMath::Clamp(SplA.FallOffCurve->GetFloatValue(GridOffset), -1.f, 1.f);
					}
					else
					{
						float LocalAlpha = FMath::Clamp(SplA.FallOffCurve->GetFloatValue(GridOffset), -1.f, 1.f);
						VelocityLerp = 1.f - LocalAlpha;
						LocalVector = FMath::Lerp(SplineDirection * LocalAlpha, LocalDelta.GetSafeNormal() * LocalAlpha * FMath::Sign(VelocityLerp), FMath::Abs(VelocityLerp));
					}
					LocalVector *= SplA.AdvancedSplineProperty.SplineWeight;
					
					if (SplA.AdvancedSplineProperty.TwistCurve != nullptr)
					{
						TwistAngle = FMath::Clamp(SplA.AdvancedSplineProperty.TwistCurve->GetFloatValue(DistanceAlongSpline / Spl->GetSpline()->GetSplineLength()), -90.f, 90.f);
						LocalVector = LocalVector.RotateAngleAxis(TwistAngle, LocalDelta.GetSafeNormal());
					}

					if (SplA.AdvancedSplineProperty.SpeedCurve != nullptr)
					{
						SpeedScale = FMath::Clamp(SplA.AdvancedSplineProperty.SpeedCurve->GetFloatValue(DistanceAlongSpline / Spl->GetSpline()->GetSplineLength()), -1.f, 1.f);
						LocalVector *= SpeedScale;
						LocalVector *= FMath::FRandRange(FMath::Min(SplA.AdvancedSplineProperty.MinSpeedRandom, SplA.AdvancedSplineProperty.MaxSpeedRandom), FMath::Max(SplA.AdvancedSplineProperty.MinSpeedRandom, SplA.AdvancedSplineProperty.MaxSpeedRandom));
					}

					if (SplA.AdvancedSplineProperty.RandomInConeAngle > 0.f)
						LocalVector = FMath::VRandCone(LocalVector.GetSafeNormal(), SplA.AdvancedSplineProperty.RandomInConeAngle / 180.f * PI) * LocalVector.Size();

					switch (SplA.AdvancedSplineProperty.AdditionalMethod)
					{
					case ESplineCombineMode::Add: 
						Vectors[i] += LocalVector;
						break;
					case ESplineCombineMode::Average:
						Vectors[i] += LocalVector;
						bNeedAverage = true;
						break;
					case ESplineCombineMode::Min:
						Vectors[i] = Vectors[i].Size() < LocalVector.Size() ? Vectors[i] : LocalVector;
						break;
					case ESplineCombineMode::Multiply:
						Vectors[i] *= LocalVector;
						break;
					case ESplineCombineMode::Max:
						Vectors[i] = Vectors[i].Size() > LocalVector.Size() ? Vectors[i] : LocalVector;;
						break;
					}
					if (Vectors[i].Size() > MaxVectorLength)
						MaxVectorLength = Vectors[i].Size();
				}
			}
		}
		if (bNeedAverage)
			for (int32 i = 0; i < NumGrids; i++)
				Vectors[i] /= 2.f;//VFSplinesArray.Num();
		bNeedAverage = false;
	}

	for (int32 i = 0; i < NumGrids; i++)
	{
		if (bUseNormalize)
			Vectors[i] /= MaxVectorLength;
	
		if (Vectors[i].Size() > 0.f)
		{
			if (SkipGridsNum > 0)
			{
				FVector GridLocation = FVector::ZeroVector;
				GridLocation.X = FMath::RoundToZero(FMath::Fmod(i, Grid.X));
				GridLocation.Y = FMath::RoundToZero(FMath::Fmod(i / Grid.X, Grid.Y));
				GridLocation.Z = FMath::RoundToZero(FMath::Fmod(i / Grid.X / Grid.Y, Grid.Z));

				if (int(GridLocation.X) % (SkipGridsNum + 1) == 0 ||
					int(GridLocation.Y) % (SkipGridsNum + 1) == 0 ||
					int(GridLocation.Z) % (SkipGridsNum + 1) == 0)
					NumArrows++;
			}
			else NumArrows++;
		}
	}

	if (bShowVectorsInSphere)
		ShowVectors();
	else
		HoldDraw = false;

	UE_LOG(LogTemp, Warning, TEXT("Calculate Complite!"));
	return;
}

float AVFBound::GetDistanceAlongSpline(const USplineComponent* InSpline, const FVector& InLocation)
{
	float InputKeyFloat = InSpline->FindInputKeyClosestToWorldLocation(InLocation);
	int32 InputKey = FMath::TruncToInt(InputKeyFloat);
	float A = InSpline->GetDistanceAlongSplineAtSplinePoint(InputKey);
	float B = InSpline->GetDistanceAlongSplineAtSplinePoint(InputKey + 1);

	return (A + ((B - A) * (InputKeyFloat - InputKey)));
}

void AVFBound::ShowVectors()
{
	RemoveVectors();
//	UE_LOG(LogTemp, Warning, TEXT("Draw Vectors"));
	int32 Locali = 0;

	for (int32 i = Locali; i < NumGrids; i++)
	{
		if (!Vectors.IsValidIndex(i))
			break;
		if (Vectors[i].Size() < 0.01f)
			continue;

		FVector GridLocation = FVector::ZeroVector;
		GridLocation.X = FMath::RoundToZero(FMath::Fmod(i, Grid.X));
		GridLocation.Y = FMath::RoundToZero(FMath::Fmod(i / Grid.X, Grid.Y));
		GridLocation.Z = FMath::RoundToZero(FMath::Fmod(i / Grid.X / Grid.Y, Grid.Z));

		if (SkipGridsNum > 0 &&
			(int(GridLocation.X) % (SkipGridsNum + 1) != 0 ||
			 int(GridLocation.Y) % (SkipGridsNum + 1) != 0 ||
			 int(GridLocation.Z) % (SkipGridsNum + 1) != 0))
			continue;

		GridLocation *= Extends * 2.f / Grid;
		GridLocation = GridLocation - Extends + Extends / Grid;

		if (bShowVectorsInSphere && FVector(GridLocation - CenterShowSphere).Size() > SphereRadius)
			continue;

	 	if (!GridArrows.IsValidIndex(Locali))
	  	{
			GridArrows.Add(NewObject<UArrowComponent>(this));
//			GridArrows[Locali]->RegisterComponent();
			GridArrows[Locali]->AttachToComponent(BoundBox, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true));
		}
		GridArrows[Locali]->SetRelativeLocation(GridLocation);
		GridArrows[Locali]->SetWorldRotation(Vectors[i].ToOrientationRotator());
		GridArrows[Locali]->ArrowSize = Vectors[i].Size() / MaxVectorLength * VectorSize;
		GridArrows[Locali]->SetRelativeScale3D(FVector(VectorLengthScale, 1.f, 1.f) * GridExtends.GetMin() / 30.f);
		GridLocation = Vectors[i].GetSafeNormal() * 255.f; //Now for color
		GridArrows[Locali]->ArrowColor = FColor(FMath::Abs(GridLocation.X), FMath::Abs(GridLocation.Y), FMath::Abs(GridLocation.Z));
		GridArrows[Locali]->RegisterComponent();
		Locali++;
	}
// 	if (Locali < GridArrows.Num())
// 	{
// 		for (int32 i = Locali; i <= GridArrows.Num() - 1.f; i++)
// 			GridArrows[i]->DestroyComponent();
// 		GridArrows.SetNum(Locali);
// 	}
// 	RegisterAllComponents();
}

void AVFBound::SaveToFile()
{
	if (Vectors.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Need generate vector field first!"));
		return;
	}
	if (FileName == "")
	{
		UE_LOG(LogTemp, Error, TEXT("FileName is empty!"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Save %s.fga in Save folder"), *FileName);
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + FString::Printf(TEXT("/%s.fga"), *FileName);
	FString FileContent = FString::Printf(TEXT("%f, %f, %f,\n %f, %f, %f,\n %f, %f, %f,\n"), Grid.X, Grid.Y, Grid.Z, -Extends.X + PivotPointOffset.X, -Extends.Y + PivotPointOffset.Y, -Extends.Z + PivotPointOffset.Z, Extends.X + PivotPointOffset.X, Extends.Y + PivotPointOffset.Y, Extends.Z + PivotPointOffset.Z);
	for (int32 i = 0; i < NumGrids; i++)
	{
		Vectors[i] = GetActorRotation().UnrotateVector(Vectors[i]);
		FileContent += FString::Printf(TEXT("%f, %f, %f,\n"), Vectors[i].X, Vectors[i].Y, Vectors[i].Z);
	}
	FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_None);
//	UVectorFieldStaticFactory::FactoryCanImport(FilePath);
}
#endif //WITH_EDITORONLY_DATA