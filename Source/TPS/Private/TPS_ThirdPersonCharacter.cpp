// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPS_ThirdPersonCharacter.h"
#include "..\Public\TPS_Weapon.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TPS_Projectile.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATPS_ThirdPersonCharacter::ATPS_ThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false; //player follow camera mouvement
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATPS_ThirdPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if(WeaponInHandsClass)
	{
		if(UWorld* World = GetWorld())
		{
			WeaponInHands = World->SpawnActor<ATPS_Weapon>(WeaponInHandsClass);
		}
	}
	InitWeapon();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPS_ThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPS_ThirdPersonCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPS_ThirdPersonCharacter::Look);

		//Tir
		PlayerInputComponent->BindAction("Tir", IE_Pressed, this, &ATPS_ThirdPersonCharacter::Tir);

		//Aiming
		PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &ATPS_ThirdPersonCharacter::StartAiming);
		PlayerInputComponent->BindAction("Aiming", IE_Released, this, &ATPS_ThirdPersonCharacter::StopAiming);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATPS_ThirdPersonCharacter::InitWeapon()
{
	if (WeaponInHands != nullptr)
	{
		// Set weapon location as character hands
		WeaponInHands->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName("Weapon_Socket"));
		// set appropriate collision settings to avoid collision with player
		WeaponInHands->SetMeshCollision(ECollisionResponse::ECR_Overlap);
		if (!bIsWeaponInHands)
		{
			bIsWeaponInHands = true;
		}
	}
}

void ATPS_ThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATPS_ThirdPersonCharacter::Look(const FInputActionValue& Value)
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

void ATPS_ThirdPersonCharacter::Tir()
{
	if(GetWorld())
	{
		if(WeaponInHands && bIsWeaponInHands)
		{
			if (WeaponInHands->ProjectileClass == nullptr) return;
			if (WeaponInHands->WeaponMesh == nullptr) return;
			
			FRotator MuzzleRotation;
			auto CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			if(CameraManager)
			{
				MuzzleRotation = CameraManager->GetActorForwardVector().Rotation();
				//MuzzleRotation.Pitch += 1.7f;
				//MuzzleRotation.Yaw += 0.4f;
			}
			FVector MuzzleLocation = WeaponInHands->WeaponMesh->GetSocketLocation("Muzzle");

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Owner = this;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			GetWorld()->SpawnActor<ATPS_Projectile>(WeaponInHands->ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
		}
	}
}

void ATPS_ThirdPersonCharacter::StartAiming()
{
	if(USpringArmComponent* Camera = GetCameraBoom())
	{
		Camera->TargetArmLength = 250.0f;
		//Camera->TargetOffset = FVector(0.0f, 0.0f, 20.0f);

		if(auto CharacterMouvement = GetCharacterMovement())
		{
			CharacterMouvement->MaxWalkSpeed = 100.0f;
		}

		bUseControllerRotationYaw = true;
		IsAiming = true;
	}
}

void ATPS_ThirdPersonCharacter::StopAiming()
{
	if(USpringArmComponent* Camera = GetCameraBoom())
	{
		Camera->TargetArmLength = 400.0f;
		//Camera->TargetOffset = FVector(0.0f, 0.0f, 0.0f);

		if(auto CharacterMouvement = GetCharacterMovement())
		{
			CharacterMouvement->MaxWalkSpeed = 500.0f;
		}

		bUseControllerRotationYaw = false;
		IsAiming = false;
	}
}
