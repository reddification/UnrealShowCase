// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "Components/Movement/BasePawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CommonConstants.h"
#include "GameFramework/SpringArmComponent.h"

ABasePawn::ABasePawn()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->bUsePawnControlRotation = 1;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetRelativeLocation(FVector(0, 0, 50));
	CameraComponent->SetRelativeRotation(FRotator(-20, 0, 0));

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif
}

// Called to bind functionality to input
void ABasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABasePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABasePawn::Jump);
}

void ABasePawn::MoveForward(float val)
{
	InputForward = val;
	if (val != 0.0f)
	{
		// AddMovementInput(CurrentViewActor->GetActorForwardVector(), val);
		FVector direction = CurrentViewCameraComponent == nullptr
			? GetActorForwardVector()
			: CurrentViewCameraComponent->GetForwardVector();
		
		AddMovementInput(direction, val);
	}
}

void ABasePawn::MoveRight(float val)
{
	InputRight = val;
	if (val != 0.0f)
	{
		// AddMovementInput(CurrentViewActor->GetActorRightVector(), val);
		FVector direction = CurrentViewCameraComponent == nullptr
			? GetActorRightVector()
			: CurrentViewCameraComponent->GetRightVector();
		AddMovementInput(direction, val);
	}
}

void ABasePawn::Jump()
{
	//why not make the component field itself of this type?
  	checkf(MovementComponent->IsA<UBasePawnMovementComponent>(),
  		TEXT("Only UGCBasePawnMovementComponent supported for JUMP"));
	UBasePawnMovementComponent* GCMovementComponent = static_cast<UBasePawnMovementComponent*>(MovementComponent);
	GCMovementComponent->JumpStart();
}

void ABasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CurrentViewActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));
}

void ABasePawn::OnBlendComplete()
{
	CurrentViewActor = GetController()->GetViewTarget();
	auto viewActorCameraComponent = CurrentViewActor->GetComponentByClass(TSubclassOf<UCameraComponent>());
	CurrentViewCameraComponent = viewActorCameraComponent != nullptr
		? static_cast<UCameraComponent*>(viewActorCameraComponent)
		: nullptr;
	
	UE_LOG(LogTemp, Verbose, TEXT("AGameCodeBasepawn::OnBlendComplete. Current view actor: %s"),
		*CurrentViewActor->GetName());
}



