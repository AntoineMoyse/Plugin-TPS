// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\TPS_Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ATPS_Projectile::ATPS_Projectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if(!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Root Component"));
	}

	if(!CollisionSphere)
	{
		// Use a sphere as a simple collision representation
		CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
		CollisionSphere->InitSphereRadius(5.0f);
		CollisionSphere->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		
		CollisionSphere->OnComponentHit.AddDynamic(this, &ATPS_Projectile::OnHit);		// set up a notification for when this component hits something blocking

		// Players can't walk on it
		CollisionSphere->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
		CollisionSphere->CanCharacterStepUpOn = ECB_No;

		// Set as root component
		RootComponent = CollisionSphere;
	}
	
	if(!ProjectileMovement)
	{
		// Use a ProjectileMovementComponent to govern this projectile's movement
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Mouvement"));
		ProjectileMovement->SetUpdatedComponent(CollisionSphere);
		ProjectileMovement->InitialSpeed = 3000.f;
		ProjectileMovement->MaxSpeed = 3000.f;
		ProjectileMovement->bRotationFollowsVelocity = true;
		ProjectileMovement->bShouldBounce = true;
		ProjectileMovement->Bounciness = 0.0f;
		ProjectileMovement->ProjectileGravityScale = 0.0f;
	}

	if(!ProjectileMesh)
	{
		ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh Component"));
		//For the mesh to follow the movement
		ProjectileMesh->AttachToComponent(CollisionSphere, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void ATPS_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATPS_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}