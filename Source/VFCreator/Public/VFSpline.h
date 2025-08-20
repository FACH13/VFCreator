// Copyright (C) 2019-2024 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "VFSpline.generated.h"

class UArrowComponent;

UCLASS()
class VFCREATOR_API AVFSpline : public AActor
{
	GENERATED_UCLASS_BODY()
		
public:	
	// Sets default values for this actor's properties
	//AVFSpline();

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFCreator")
	class USplineComponent* SplineComponent;

	FORCEINLINE USplineComponent* GetSpline() const { return SplineComponent; }

private:

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UArrowComponent* ArrowComponent;
#endif
};
