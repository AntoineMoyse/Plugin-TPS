// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPS_Projectile.generated.h"

UCLASS()
class TPS_API ATPS_Projectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATPS_Projectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	class UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "ProjectileComponent")
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ProjectileComponent")
	class UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
};
