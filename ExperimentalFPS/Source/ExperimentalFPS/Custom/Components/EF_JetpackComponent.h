// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EF_JetpackComponent.generated.h"

class AExperimentalFPSCharacter;
class UCharacterMovementComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EXPERIMENTALFPS_API UEF_JetpackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEF_JetpackComponent();

	/** Upwards Strenght of the jetpack, the more it is, the bigger is the acceleration for the jetpack, if its too low, the gravity has more power and you dont even fly */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackStrenght = 2000.0f;

	/** maximum fuel for the jetpack, this goes in seconds, and its depleted with simple time, so if its 2, you can only fly for 2 seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackMaxFuel = 2.0f;

	/** Multiplier for the jetpack fuel regeneration, uses the time, if its 0.5, and the JetpackMaxFuel is 2 seconds, that means that it will take 4 seconds to be back at 100% */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS")
	float JetpackRefuelRate = 0.5f;

	/**This is the time the jetpack has to cool down, when you deplete it completely */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JR_FPS")
	float JetpackRecovery = 2.0f;

	/** Holds the current fuel amount */
	UPROPERTY(Transient)
	float CurrentFuel = 0.0f;

	UPROPERTY(Transient)
	bool bIsJetpackEnabled = true;

	UPROPERTY(Transient)
	bool bIsBeingUsed = false;

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "JR_FPS")
	void AttachJetpack(AExperimentalFPSCharacter* TargetCharacter);

	void UpdateMovement(float DeltaTime);

	void DisableJetpack();
	void EnableJetpack();

	void SetbIsBeingUsed(bool bInIsBeingUsed);

	void StartJetpack();
	void StopJetpack();

private:
	/** The Character using this Jetpack*/
	UPROPERTY(Transient)
	TObjectPtr<AExperimentalFPSCharacter> Character = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComp = nullptr;

protected:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
