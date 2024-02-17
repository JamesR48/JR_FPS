// Fill out your copyright notice in the Description page of Project Settings.


#include "Custom/Components/EF_JetpackComponent.h"
#include "EF_JetpackComponent.h"

#include "ExperimentalFPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UEF_JetpackComponent::UEF_JetpackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CurrentFuel = JetpackMaxFuel;
}

void UEF_JetpackComponent::AttachJetpack(AExperimentalFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no rifle yet
	if (Character == nullptr || Character->GetHasJetpack())
	{
		return;
	}

	CharacterMovementComp = Character->GetCharacterMovement();

	// Attach the weapon to the First Person Character
	//FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	Character->SetHasJetpack(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			//Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			//EnhancedInputComponent->RemoveActionBinding(Character->JumpActionBinding, 0); //BindAction(Character->JumpAction, ETriggerEvent::Started, this, &UEF_JetpackComponent::StartJetpack);
			//Character->JumpActionBinding = EnhancedInputComponent->BindAction(Character->JumpAction, ETriggerEvent::Completed, this, &UEF_JetpackComponent::StopJetpack);
		}
	}
}

void UEF_JetpackComponent::UpdateMovement(float DeltaTime)
{
	if (Character && CharacterMovementComp)
	{
		if (bIsBeingUsed)
		{
			// make fuel decrease, as you are flying
			CurrentFuel -= DeltaTime;
			// jetpack is depleted, disable it and stop flying
			if (CurrentFuel < 0)
			{
				//ShooterCharacterOwner->StopJetpack();
				DisableJetpack();
			}
			// Add some acceleration to the Upward velocity, so the character is propulsed upwards
			CharacterMovementComp->Velocity.Z += JetpackStrenght * DeltaTime;
		}
		else if (bIsJetpackEnabled)
		{
			// only refuel the jetpack when you are not flying and the jetpack is enabled
			CurrentFuel += DeltaTime * JetpackRefuelRate;
			if (CurrentFuel >= JetpackMaxFuel)
			{
				CurrentFuel = JetpackMaxFuel;
			}
		}
	}
}

void UEF_JetpackComponent::DisableJetpack()
{
	// Debug Message
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("JetpackDisabled"));
	bIsJetpackEnabled = false;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEF_JetpackComponent::EnableJetpack, JetpackRecovery, false);
}

void UEF_JetpackComponent::EnableJetpack()
{
	// Debug Message
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("JetpackEnabled"));
	bIsJetpackEnabled = true;
}

void UEF_JetpackComponent::SetbIsBeingUsed(bool bInIsBeingUsed)
{
	bIsBeingUsed = bInIsBeingUsed;
}

void UEF_JetpackComponent::StartJetpack()
{
	if (bIsJetpackEnabled) // check for if it is enabled, so you cant start flying again while the jetpack disabled due to the player depleting it
	{
		SetbIsBeingUsed(true);
	}
}

void UEF_JetpackComponent::StopJetpack()
{
	SetbIsBeingUsed(false);
}

// Called every frame
void UEF_JetpackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateMovement(DeltaTime);
}

