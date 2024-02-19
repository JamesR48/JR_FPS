// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExperimentalFPSCharacter.h"
#include "ExperimentalFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

// -------------- JR_FPS Modifications START -------------- //
#include "GameFramework/CharacterMovementComponent.h"
#include "Custom/Components/EF_DashComponent.h"
// -------------- JR_FPS Modifications END -------------- //

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AExperimentalFPSCharacter

AExperimentalFPSCharacter::AExperimentalFPSCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	
// -------------- JR_FPS Modifications START -------------- //
	DashComponent = CreateDefaultSubobject<UEF_DashComponent>(TEXT("DashComponent"));
	bHasJetpack = false;
// -------------- JR_FPS Modifications END -------------- //
}

void AExperimentalFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

// -------------- JR_FPS Modifications START -------------- //
	if(GetCharacterMovement())
	{
		DefaultAirControl = GetCharacterMovement()->AirControl;
		DefaultGravityScale = GetCharacterMovement()->GravityScale;
	}
// -------------- JR_FPS Modifications END -------------- //

}

//////////////////////////////////////////////////////////////////////////// Input

void AExperimentalFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AExperimentalFPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AExperimentalFPSCharacter::Look);

// -------------- JR_FPS Modifications START -------------- //
		if (IsValid(DashComponent) && IsValid(DashComponent->DashAction))
		{
			EnhancedInputComponent->BindAction(DashComponent->DashAction, ETriggerEvent::Triggered, this, &AExperimentalFPSCharacter::Dash);
		}
// -------------- JR_FPS Modifications END -------------- //
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AExperimentalFPSCharacter::Move(const FInputActionValue& Value)
{
	if (IsValid(DashComponent) && DashComponent->GetIsDashing()) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	InputMovementVector = MovementVector;

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AExperimentalFPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AExperimentalFPSCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AExperimentalFPSCharacter::GetHasRifle()
{
	return bHasRifle;
}

// -------------- JR_FPS Modifications START -------------- //

void AExperimentalFPSCharacter::Dash(const FInputActionValue& Value)
{
	if (IsValid(DashComponent) && IsValid(GetCharacterMovement()))
	{
		FVector InputVector = (GetActorForwardVector() * InputMovementVector.Y) + (GetActorRightVector() * InputMovementVector.X);
		FVector DashVel = GetCharacterMovement()->Velocity.Length() * InputVector;
		DashComponent->OnStartDash(DashVel);
	}
}

void AExperimentalFPSCharacter::Landed(const FHitResult &Hit)
{
	Super::Landed(Hit);

	if(bHasJetpack && GetCharacterMovement())
	{
		GetCharacterMovement()->AirControl = DefaultAirControl;
		GetCharacterMovement()->GravityScale = DefaultGravityScale;
	}
}

const FRotator AExperimentalFPSCharacter::GetLookAtRotation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		return PlayerController->PlayerCameraManager->GetCameraRotation();
	}

	return FRotator::ZeroRotator;
}

void AExperimentalFPSCharacter::SetHasJetpack(bool bInHasJetpack)
{
	bHasJetpack = bInHasJetpack;
}

bool AExperimentalFPSCharacter::GetHasJetpack()
{
	return bHasJetpack;
}

// -------------- JR_FPS Modifications END -------------- //
