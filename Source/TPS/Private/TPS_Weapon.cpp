// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\TPS_Weapon.h"

#include "..\Public\TPS_Projectile.h"


// Sets default values
ATPS_Weapon::ATPS_Weapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	Projectile = CreateDefaultSubobject<ATPS_Projectile>(TEXT("Projectile"));

	SetRootComponent(WeaponMesh);
}

// Called when the game starts or when spawned
void ATPS_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

FRotator ATPS_Weapon::GetMuzzleRotation()
{
	if(WeaponMesh)
	{
		return WeaponMesh->GetSocketRotation("Muzzle");
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Muzzle socket is not specified, verify skeletal mesh"))
		return GetActorRotation();
	}
}

void ATPS_Weapon::SetMeshCollision(ECollisionResponse Response)
{
	WeaponMesh->SetCollisionResponseToAllChannels(Response);
}