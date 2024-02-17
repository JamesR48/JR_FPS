// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "EF_AttractorComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EXPERIMENTALFPS_API UEF_AttractorComponent : public USphereComponent
{
	GENERATED_BODY()
	
public:

	UEF_AttractorComponent();
};
