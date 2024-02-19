// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "EF_JetpackComponent.generated.h"

class AExperimentalFPSCharacter;
class UCharacterMovementComponent;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFuelChanged, const float, FuelValue);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EXPERIMENTALFPS_API UEF_JetpackComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:	
	UEF_JetpackComponent();

	/** Jetpack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JetpackAction = nullptr;

	/** Upwards Strength of the jetpack, the more it is, the bigger is the acceleration for the jetpack, if its too low, the gravity has more power and you dont even fly */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackStrength = 500.0f;

	/** maximum fuel for the jetpack, this goes in seconds, and its depleted with simple time, so if its 2, you can only fly for 2 seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS", meta = (ClampMin = 0.1f, UIMin = 0.1f))
	float JetpackMaxFuel = 2.0f;

	/** Multiplier for the jetpack fuel regeneration, uses the time, if its 0.5, and the JetpackMaxFuel is 2 seconds, that means that it will take 4 seconds to be back at 100% */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackRefuelRate = 0.5f;

	/**This is the time the jetpack has to cool down, when you deplete it completely */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackRecovery = 2.0f;

	/**Override for CharacterMovement's AirControl */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackAirControl = 0.8f;

	/**Override for CharacterMovement's GravityScale */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackGravityScale = 0.4f;

	/**For clamping the speed when ascending */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackMaxZVelocity = 480.0f;

	/**For clamping the speed when falling */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackMinZVelocity = -480.0f;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "JR_FPS")
	FOnFuelChanged OnFuelChanged;

	UPROPERTY(Transient)
	float CurrentFuel = 0.0f;

	UPROPERTY(Transient)
	bool bIsJetpackEnabled = true;

	UPROPERTY(Transient)
	bool bIsBeingUsed = false;

	UFUNCTION(BlueprintCallable, Category = "JR_FPS")
	void AttachJetpack(AExperimentalFPSCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "JR_FPS")
	void StartJetpack();

	UFUNCTION(BlueprintCallable, Category = "JR_FPS")
	void StopJetpack();

	void UpdateMovement(float DeltaTime);

	void DisableJetpack();
	void EnableJetpack();

	void SetbIsBeingUsed(bool bInIsBeingUsed);

private:
	/** The Character using this Jetpack*/
	UPROPERTY(Transient)
	TObjectPtr<AExperimentalFPSCharacter> Character = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComp = nullptr;

protected:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
