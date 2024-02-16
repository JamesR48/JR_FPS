// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EF_DashComponent.generated.h"

class UInputAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EXPERIMENTALFPS_API UEF_DashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEF_DashComponent();

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr <UInputAction> DashAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS", meta = (AllowPrivateAccess = "true", ClampMin = 1.0f, UIMin = 1.0f))
	float DashSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS", meta = (AllowPrivateAccess = "true", ClampMin = 1.0f, UIMin = 1.0f))
	float DashDistance = 500.0f;

protected:
	UPROPERTY(VisibleAnywhere, Category = "JR_FPS", meta = (AllowPrivateAccess = "true"))
	bool bIsDashing = false;

	FVector CurrentDashingVelocity = FVector::ZeroVector;
	float CurrentDashingTime = 0.0f;
	FVector DashEndLocation = FVector::ZeroVector;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetIsDashing(const bool InIsDashing);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnStartDash(const FVector& InVelocity);
	void UpdateDash(float DeltaTime);
	void OnEndDash();

	UFUNCTION(BlueprintCallable)
	const bool GetIsDashing() { return bIsDashing; }
};
