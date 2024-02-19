// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AExperimentalFPSCharacter;

// -------------- JR_FPS Modifications START -------------- //
class UEF_AttractorComponent;
// -------------- JR_FPS Modifications END -------------- //

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EXPERIMENTALFPS_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AExperimentalFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AExperimentalFPSCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	
public:
// -------------- JR_FPS Modifications START -------------- //

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS/GravityGun", meta = (AllowPrivateAccess = "true"))
	bool bIsGravityGun = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS/GravityGun", meta = (AllowPrivateAccess = "true", EditCondition = "bIsGravityGun", EditConditionHides, ClampMin = 1.0f, UIMin = 1.0f))
	float FiringForce = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS/GravityGun", meta = (AllowPrivateAccess = "true", EditCondition = "bIsGravityGun", EditConditionHides, ClampMin = 1.0f, UIMin = 1.0f))
	float AttractForce = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JR_FPS/GravityGun", meta = (AllowPrivateAccess = "true", EditCondition = "bIsGravityGun", EditConditionHides, ClampMin = 1.0f, UIMin = 1.0f))
	float PickUpRadius = 1.0f;

	/** Gravity Attraction Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true", EditCondition = "bIsGravityGun", EditConditionHides))
	class UInputAction* AttractAction;

	UPROPERTY(Transient)
	TObjectPtr<AActor> CurrentAttractedActor = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<AActor> CurrentPickedActor = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UEF_AttractorComponent> CurrentAttractorComponent = nullptr;

	UPROPERTY(Transient)
	float TimeToAttractActor = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "JR_FPS/GravityGun")
	void Attract();

	UFUNCTION(BlueprintCallable, Category = "JR_FPS/GravityGun")
	void OnStartAttract();

	UFUNCTION(BlueprintCallable, Category = "JR_FPS/GravityGun")
	void OnEndAttract();

	UFUNCTION(BlueprintCallable, Category = "JR_FPS/GravityGun")
	void OnPickAttractor(AActor* InActorToPick);

	UFUNCTION(BlueprintCallable, Category = "JR_FPS/GravityGun")
	void OnDropAttractor(const FVector& InDropForce = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "JR_FPS/GravityGun")
	void OnShootAttracted();

	void UpdateAttractor(float DeltaTime);

	const FVector GetFiringLocation();

	void EnableActorComponentPhysics(AActor* InActor, const FVector& InForceOnEnable = FVector::ZeroVector);

// -------------- JR_FPS Modifications END -------------- //

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** The Character holding this weapon*/
	AExperimentalFPSCharacter* Character;
};
