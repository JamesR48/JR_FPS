// Fill out your copyright notice in the Description page of Project Settings.


#include "custom/Components/EF_DashComponent.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UEF_DashComponent::UEF_DashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEF_DashComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEF_DashComponent::SetIsDashing(const bool InIsDashing)
{
	bIsDashing = InIsDashing;
}


// Called every frame
void UEF_DashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDashing)
	{
		UpdateDash(DeltaTime);
	}
}

void UEF_DashComponent::OnStartDash(const FVector& InVelocity)
{
	if (!GetOwner() || InVelocity.SquaredLength() <= SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Warning, TEXT("JR_FPS - You must be moving to Dash"));
		return;
	}

	SetIsDashing(true);

	CurrentDashingTime = DashDistance / DashSpeed;
	FVector MoveDirection = InVelocity.GetSafeNormal();
	FVector StartLocation = GetOwner()->GetActorLocation();
	CurrentDashingVelocity = DashSpeed * MoveDirection;
	DashEndLocation = StartLocation + MoveDirection * DashDistance;

	FHitResult DashHit;
	FCollisionShape DashCollision = FCollisionShape::MakeSphere(20.0f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	const bool bIsHit = GetWorld()->SweepSingleByChannel(DashHit, StartLocation, DashEndLocation, FQuat::Identity, ECC_WorldStatic, DashCollision, Params);
	if (bIsHit)
	{
		DashEndLocation = DashHit.Location;
		float DistanceOnHit = (DashEndLocation - StartLocation).Length();
		CurrentDashingTime = DistanceOnHit / DashSpeed;
	}
}

void UEF_DashComponent::UpdateDash(float DeltaTime)
{
	if (GetOwner() && CurrentDashingTime > 0.0f)
	{
		const FVector NewLocation = GetOwner()->GetActorLocation() + CurrentDashingVelocity * DeltaTime;
		GetOwner()->SetActorLocation(NewLocation);

		CurrentDashingTime -= DeltaTime;
		if (CurrentDashingTime <= 0.0f)
		{
			CurrentDashingTime = 0.0f;
			OnEndDash();
		}
	}
}

void UEF_DashComponent::OnEndDash()
{
	if (GetOwner())
	{
		GetOwner()->SetActorLocation(DashEndLocation);
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			OwnerCharacter->ResetJumpState();
		}
	}
	SetIsDashing(false);
}

