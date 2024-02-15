// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EF_GravityGun.generated.h"

//class TP_WeaponComponent;

UCLASS()
class EXPERIMENTALFPS_API AEF_GravityGun : public AActor
{
	GENERATED_BODY()
	
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JR_FPS", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<TP_WeaponComponent> WeaponComponent = nullptr;

public:	
	// Sets default values for this actor's properties
	AEF_GravityGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
