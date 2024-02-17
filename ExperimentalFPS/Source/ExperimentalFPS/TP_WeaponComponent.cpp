// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "ExperimentalFPSCharacter.h"
#include "ExperimentalFPSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Custom/Components/EF_AttractorComponent.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (!bIsGravityGun && ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = Character->GetLookAtRotation();
			const FVector SpawnLocation = GetFiringLocation();
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<AExperimentalFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
	
	if (CurrentPickedActor)
	{
		OnShootAttracted();
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::Attract()
{
	if (!CurrentPickedActor && !CurrentAttractedActor)
	{
		OnStartAttract();
	}
	else if(CurrentPickedActor)
	{
		OnDropAttractor();
	}
}

void UTP_WeaponComponent::OnStartAttract()
{
	if (Character && IsValid(GetOwner()) && IsValid(GetWorld()))
	{
		const FRotator CamRotation = Character->GetLookAtRotation();
		FVector AttractedLocation = GetFiringLocation();
		FVector TraceEnd = AttractedLocation + CamRotation.Vector() * PickUpRadius * 8.0f;

		FHitResult Hit;
		FCollisionObjectQueryParams ObjectQuery;
		ObjectQuery.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		if (GetWorld()->LineTraceSingleByObjectType(Hit, AttractedLocation, TraceEnd, ObjectQuery, Params))
		{
			if (Hit.GetActor() && Hit.GetComponent())
			{
				CurrentAttractorComponent = Cast<UEF_AttractorComponent>(Hit.GetComponent());
				if (CurrentAttractorComponent)
				{
					DrawDebugSphere(GetWorld(), Hit.GetActor()->GetActorLocation(), 100.0f, 12, FColor::Red, false, 5.0f);
					float DistToAttractSqr = (Hit.GetActor()->GetActorLocation() - AttractedLocation).SquaredLength();
					if (DistToAttractSqr <= PickUpRadius * PickUpRadius)
					{
						OnPickAttractor(Hit.GetActor());
					}
					else
					{
						FVector StartPickUpLocation = AttractedLocation + CamRotation.Vector() * PickUpRadius;
						float DistToPickUp = (Hit.GetActor()->GetActorLocation() - StartPickUpLocation).Length();
						CurrentAttractedActor = Hit.GetActor();
						TimeToAttractActor = DistToPickUp / AttractForce;
						CurrentAttractedActor->DisableComponentsSimulatePhysics();
					}
				}
			}
		}
	}
}

void UTP_WeaponComponent::OnEndAttract()
{
	if (CurrentAttractedActor)
	{
		if (!CurrentPickedActor)
		{
			EnableActorComponentPhysics(CurrentAttractedActor);
			CurrentAttractorComponent = nullptr;
		}

		CurrentAttractedActor = nullptr;
		TimeToAttractActor = 0.0f;
	}
}

void UTP_WeaponComponent::OnPickAttractor(AActor* InActorToPick)
{
	if (InActorToPick)
	{
		CurrentPickedActor = InActorToPick;
		CurrentPickedActor->SetActorLocation(GetFiringLocation());
		CurrentPickedActor->DisableComponentsSimulatePhysics();
		CurrentPickedActor->SetActorEnableCollision(false);
		FAttachmentTransformRules AttachRules = FAttachmentTransformRules::KeepWorldTransform;
		CurrentPickedActor->AttachToComponent(this, AttachRules);
	}
}

void UTP_WeaponComponent::OnDropAttractor(const FVector& InDropForce)
{
	if (CurrentPickedActor)
	{
		CurrentPickedActor->DetachRootComponentFromParent();
		EnableActorComponentPhysics(CurrentPickedActor, InDropForce);
		CurrentPickedActor = nullptr;
		CurrentAttractorComponent = nullptr;
	}
}

void UTP_WeaponComponent::OnShootAttracted()
{
	if (Character)
	{
		FVector Force = Character->GetLookAtRotation().Vector() * FiringForce;
		OnDropAttractor(Force);
	}
}

const FVector UTP_WeaponComponent::GetFiringLocation()
{
	if (Character)
	{
		const FRotator CamRotation = Character->GetLookAtRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector FiringLocation = GetOwner()->GetActorLocation() + CamRotation.RotateVector(MuzzleOffset);
		return FiringLocation;
	}

	return FVector::ZeroVector;
}

void UTP_WeaponComponent::EnableActorComponentPhysics(AActor* InActor, const FVector& InForceOnEnable)
{
	if (InActor)
	{
		InActor->SetActorEnableCollision(true);
		for (UActorComponent* Component : InActor->GetComponents())
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
			{
				if (PrimComp != CurrentAttractorComponent)
				{
					PrimComp->SetSimulatePhysics(true);
					if (!InForceOnEnable.IsNearlyZero())
					{
						PrimComp->SetAllPhysicsLinearVelocity(InForceOnEnable);
					}
				}
			}
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(AExperimentalFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no rifle yet
	if (Character == nullptr || Character->GetHasRifle())
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);

			if (bIsGravityGun)
			{
				// Attract
				EnhancedInputComponent->BindAction(AttractAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::Attract);
				EnhancedInputComponent->BindAction(AttractAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::OnEndAttract);
			}
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentAttractedActor && TimeToAttractActor > 0.0f)
	{
		FVector AttractedLocation = GetFiringLocation();
		FVector MoveDir = (AttractedLocation - CurrentAttractedActor->GetActorLocation()).GetSafeNormal();

		FVector TempPos = CurrentAttractedActor->GetActorLocation();
		const FVector NewLocation = CurrentAttractedActor->GetActorLocation() + (MoveDir * AttractForce) * DeltaTime;
		CurrentAttractedActor->SetActorLocation(NewLocation);

		TimeToAttractActor -= DeltaTime;
		if (TimeToAttractActor <= 0.0f)
		{
			OnPickAttractor(CurrentAttractedActor);
			OnEndAttract();
		}
	}
}
