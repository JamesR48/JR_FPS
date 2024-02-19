// Fill out your copyright notice in the Description page of Project Settings.


//#include "Custom/Components/EF_JetpackComponent.h"
#include "EF_JetpackComponent.h"

#include "ExperimentalFPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UEF_JetpackComponent::UEF_JetpackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEF_JetpackComponent::AttachJetpack(AExperimentalFPSCharacter* TargetCharacter)
{
	if (TargetCharacter == nullptr || TargetCharacter->GetHasJetpack())
	{
		return;
	}
	
	CurrentFuel = JetpackMaxFuel;

	Character = TargetCharacter;

	if(Character->GetCharacterMovement())
	{
		CharacterMovementComp = Character->GetCharacterMovement();
	}

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("BackAttach")));

	Character->SetHasJetpack(true);

	if (JetpackAction)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				EnhancedInputComponent->BindAction(JetpackAction, ETriggerEvent::Started, this, &UEF_JetpackComponent::StartJetpack);
				EnhancedInputComponent->BindAction(JetpackAction, ETriggerEvent::Completed, this, &UEF_JetpackComponent::StopJetpack);
			}
		}
	}
}

void UEF_JetpackComponent::UpdateMovement(float DeltaTime)
{
	if (Character && CharacterMovementComp)
	{
		if (bIsBeingUsed)
		{
			CurrentFuel -= DeltaTime;

			if (CurrentFuel <= 0.0f)
			{
				CurrentFuel = 0.0f;
				StopJetpack();
				DisableJetpack();
			}
			
			OnFuelChanged.Broadcast(CurrentFuel);
			
			CharacterMovementComp->Velocity.Z += JetpackStrength * DeltaTime;
		}
		else if (bIsJetpackEnabled)
		{
			CurrentFuel += DeltaTime * JetpackRefuelRate;

			if (CurrentFuel >= JetpackMaxFuel)
			{
				CurrentFuel = JetpackMaxFuel;
			}

			OnFuelChanged.Broadcast(CurrentFuel);
		}
		CharacterMovementComp->Velocity.Z = FMath::Clamp(CharacterMovementComp->Velocity.Z, JetpackMinZVelocity, JetpackMaxZVelocity);
	}
}

void UEF_JetpackComponent::DisableJetpack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("JetpackDisabled"));
	bIsJetpackEnabled = false;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEF_JetpackComponent::EnableJetpack, JetpackRecovery, false);
}

void UEF_JetpackComponent::EnableJetpack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("JetpackEnabled"));
	bIsJetpackEnabled = true;
}

void UEF_JetpackComponent::SetbIsBeingUsed(bool bInIsBeingUsed)
{
	bIsBeingUsed = bInIsBeingUsed;
}

void UEF_JetpackComponent::StartJetpack()
{
	if (Character && CharacterMovementComp)
	{
		if (CharacterMovementComp->IsFalling() || CharacterMovementComp->IsFlying())
		{
			if (bIsJetpackEnabled)
			{
				CharacterMovementComp->AirControl = JetpackAirControl;
				CharacterMovementComp->GravityScale = JetpackGravityScale;
				SetbIsBeingUsed(true);
			}
		}
	}
}

void UEF_JetpackComponent::StopJetpack()
{
	if(bIsBeingUsed)
	{
		SetbIsBeingUsed(false);
	}
}

void UEF_JetpackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateMovement(DeltaTime);
}

