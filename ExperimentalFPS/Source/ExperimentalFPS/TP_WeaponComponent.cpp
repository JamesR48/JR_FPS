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
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
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
		OnPickUp(true);
	}
}

void UTP_WeaponComponent::OnStartAttract()
{
	if (Character && IsValid(GetOwner()) && IsValid(GetWorld()))
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController && PlayerController->PlayerCameraManager)
		{
			const FRotator CamRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			FVector AttractedLocation = GetOwner()->GetActorLocation() + CamRotation.RotateVector(MuzzleOffset);
			FVector TraceEnd = AttractedLocation + CamRotation.Vector() * PickUpRadius * 8.0f;
			//DrawDebugSphere(GetWorld(), TraceEnd, 20.0f, 12, FColor::Red, false, 5.0f);

			FHitResult Hit;
			FCollisionObjectQueryParams ObjectQuery;
			ObjectQuery.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
			if (GetWorld()->LineTraceSingleByObjectType(Hit, AttractedLocation, TraceEnd, ObjectQuery))
			{
				if (Hit.GetActor())
				{
					UEF_AttractorComponent* AttractorComp = Cast<UEF_AttractorComponent>(Hit.GetActor()->GetComponentByClass(UEF_AttractorComponent::StaticClass()));
					if (AttractorComp)
					{
						DrawDebugSphere(GetWorld(), Hit.GetActor()->GetActorLocation(), 100.0f, 12, FColor::Red, false, 5.0f);
						float DistToAttractSqr = (Hit.GetActor()->GetActorLocation() - AttractedLocation).SquaredLength();
						if (DistToAttractSqr <= PickUpRadius * PickUpRadius)
						{
							CurrentPickedActor = Hit.GetActor();
							CurrentPickedActor->SetActorLocation(AttractedLocation);
							CurrentPickedActor->DisableComponentsSimulatePhysics();
							CurrentPickedActor->SetActorEnableCollision(false);
							FAttachmentTransformRules AttachRules = FAttachmentTransformRules::KeepWorldTransform;
							CurrentPickedActor->AttachToComponent(this, AttachRules);
						}
						else
						{
							FVector MaxPickUpLocation = AttractedLocation + CamRotation.Vector() * PickUpRadius;
							float DistToPickUp = (Hit.GetActor()->GetActorLocation() - MaxPickUpLocation).Length();
							CurrentAttractedActor = Hit.GetActor();
							TimeToAttractActor = DistToPickUp / (FiringForce*0.4f);
							OldPos = CurrentAttractedActor->GetActorLocation();
							CurrentAttractedActor->DisableComponentsSimulatePhysics();
						}
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
			CurrentAttractedActor->SetActorEnableCollision(true);
			for (UActorComponent* Component : CurrentAttractedActor->GetComponents())
			{
				if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
				{
					PrimComp->SetSimulatePhysics(true);
				}
			}
		}

		CurrentAttractedActor = nullptr;
		TimeToAttractActor = 0.0f;
	}
}

void UTP_WeaponComponent::OnPickUp(bool bIsDropping)
{
	if (bIsDropping && CurrentPickedActor)
	{
		CurrentPickedActor->DetachRootComponentFromParent();

		CurrentPickedActor->SetActorEnableCollision(true);

		for (UActorComponent* Component : CurrentPickedActor->GetComponents())
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
			{
				PrimComp->SetSimulatePhysics(true);
			}
		}
		CurrentPickedActor = nullptr;
	}
	else
	{

	}
}

void UTP_WeaponComponent::OnShootAttracted()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		const FRotator CamRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		CurrentPickedActor->DetachRootComponentFromParent();
		CurrentPickedActor->SetActorEnableCollision(true);
		for (UActorComponent* Component : CurrentPickedActor->GetComponents())
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
			{
				PrimComp->SetSimulatePhysics(true);
				FVector Force = CamRotation.Vector() * FiringForce;
				PrimComp->SetAllPhysicsLinearVelocity(Force);
			}
		}
		CurrentPickedActor = nullptr;
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
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController && PlayerController->PlayerCameraManager)
		{
			const FRotator CamRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			FVector AttractedLocation = GetOwner()->GetActorLocation() + CamRotation.RotateVector(MuzzleOffset);
			FVector MoveDir = (AttractedLocation - CurrentAttractedActor->GetActorLocation()).GetSafeNormal();

			FVector TempPos = CurrentAttractedActor->GetActorLocation();
			// (2 * CurrentAttractedActor->GetActorLocation() - OldPos) + (MoveDir * FiringForce) * DeltaTime * DeltaTime;
			// CurrentAttractedActor->GetActorLocation() + (MoveDir * FiringForce) * DeltaTime;
			const FVector NewLocation = (2 * CurrentAttractedActor->GetActorLocation() - OldPos) + (MoveDir * FiringForce * 0.4f) * DeltaTime * DeltaTime;
			CurrentAttractedActor->SetActorLocation(NewLocation);
			OldPos = TempPos;

			TimeToAttractActor -= DeltaTime;
			if (TimeToAttractActor <= 0.0f)
			{
				CurrentPickedActor = CurrentAttractedActor;
				CurrentPickedActor->SetActorLocation(AttractedLocation);
				CurrentPickedActor->DisableComponentsSimulatePhysics();
				CurrentPickedActor->SetActorEnableCollision(false);
				FAttachmentTransformRules AttachRules = FAttachmentTransformRules::KeepWorldTransform;
				CurrentPickedActor->AttachToComponent(this, AttachRules);

				OnEndAttract();
			}
		}
	}
}
