// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPS_Weapon.generated.h"

UCLASS()
class TPS_API ATPS_Weapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATPS_Weapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponent")
	class USkeletalMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponProjectile")
	TSubclassOf<class ATPS_Projectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, Category = "WeaponProjectile")
	ATPS_Projectile* Projectile;

	UFUNCTION()
	FRotator GetMuzzleRotation();
	
	UFUNCTION()
	void SetMeshCollision(ECollisionResponse Response);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponent")
	class UBoxComponent* BoxCollision = nullptr;

public:
};
