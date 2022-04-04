#include "BaseHumanoidCharacter.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Data/Movement/MantlingMovementParameters.h"
#include "Data/Movement/Posture.h"
#include "Data/Movement/StopClimbingMethod.h"
#include "CommonConstants.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Actors/Interactive/Environment/Ladder.h"
#include "Actors/Interactive/Environment/Zipline.h"
#include "Components/Character/CharacterAttributesComponent.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Character/LedgeDetectionComponent.h"
#include "Components/Character/SoakingComponent.h"
#include "Components/Combat/CharacterCombatComponent.h"
#include "Components/Movement/HumanoidCharacterMovementComponent.h"
#include "Data/EnvironmentDamage.h"
#include "Data/MontagePlayResult.h"
#include "Data/DataAssets/Characters/HumanoidCharacterDataAsset.h"
#include "Data/Movement/ZiplineParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Net/UnrealNetwork.h"

ABaseHumanoidCharacter::ABaseHumanoidCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHumanoidCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)
		.SetDefaultSubobjectClass<UCharacterAttributesComponent>(CharacterAttributesComponentName))
{
	HumanoidMovementComponent = StaticCast<UHumanoidCharacterMovementComponent*>(GetCharacterMovement());
	// HumanoidMovementComponent->SetNetAddressable();
	// HumanoidMovementComponent->SetIsReplicated(true);
	HumanoidCharacterAttributesComponent = StaticCast<UCharacterAttributesComponent*>(CharacterAttributesComponent);
	
	LedgeDetectionComponent = CreateDefaultSubobject<ULedgeDetectionComponent>(TEXT("LedgeDetection"));
	AddOwnedComponent(LedgeDetectionComponent);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	AddOwnedComponent(InventoryComponent);
	
	SoakingComponent = CreateDefaultSubobject<USoakingComponent>(TEXT("Soaking"));
	AddOwnedComponent(SoakingComponent);

	CarryingComponent = CreateDefaultSubobject<UCarryingComponent>(TEXT("Carrying"));
	AddOwnedComponent(CarryingComponent);
	
	HumanoidMovementComponent->bNotifyApex = true;

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

void ABaseHumanoidCharacter::BeginPlay()
{
	Super::BeginPlay();
	checkf(IsValid(CharacterSettings) && CharacterSettings->IsA<UHumanoidCharacterDataAsset>(),
		TEXT("Character settings DataAsset must be a valid HumanoidCharacterDataAsset"));
	HumanoidCharacterSettings = StaticCast<const UHumanoidCharacterDataAsset*>(CharacterSettings);
	HumanoidMovementComponent->ClimbableTopReached.BindUObject(this, &ABaseHumanoidCharacter::OnClimbableTopReached);
	HumanoidMovementComponent->StoppedClimbing.BindUObject(this, &ABaseHumanoidCharacter::OnStoppedClimbing);
	HumanoidMovementComponent->CrouchedOrProned.BindUObject(this, &ABaseHumanoidCharacter::OnStartCrouchOrProne);
	HumanoidMovementComponent->WokeUp.BindUObject(this, &ABaseHumanoidCharacter::OnEndCrouchOrProne);
	HumanoidMovementComponent->SlidingStateChangedEvent.BindUObject(this, &ABaseHumanoidCharacter::OnSlidingStateChangedEvent);
	HumanoidMovementComponent->ZiplineObstacleHit.BindUObject(this, &ABaseHumanoidCharacter::OnZiplineObstacleHit);
	HumanoidMovementComponent->DesiredRotationReachedEvent.AddUObject(this, &ABaseHumanoidCharacter::OnDesiredRotationReached);
	
	MontageEndedUnlockControlsEvent.BindUObject(this, &ABaseHumanoidCharacter::OnMontageEndedUnlockControls);

	CharacterEquipmentComponent->AimingSpeedChangedEvent.BindUObject(HumanoidMovementComponent.Get(), &UHumanoidCharacterMovementComponent::SetAimingSpeed);
	CharacterEquipmentComponent->AimingSpeedResetEvent.BindUObject(HumanoidMovementComponent.Get(), &UHumanoidCharacterMovementComponent::ResetAimingSpeed);
	CharacterCombatComponent->AimStateChangedEvent.AddUObject(this, &ABaseHumanoidCharacter::OnAimStateChanged);
	
	HumanoidCharacterAttributesComponent->OutOfStaminaEvent.AddUObject(this, &ABaseHumanoidCharacter::OnOutOfStamina);
	
	UpdateStrafingControls();
}

void ABaseHumanoidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TryChangeSprintState();
	UpdateSuffocatingState();
}

#pragma region ATTRIBUTES

void ABaseHumanoidCharacter::UpdateSuffocatingState()
{
	if (HumanoidMovementComponent->IsSwimming())
	{
		FVector HeadPosition = GetMesh()->GetBoneLocation(HumanoidCharacterSettings->HeadBoneName);
		APhysicsVolume* Volume = HumanoidMovementComponent->GetPhysicsVolume();
		if (IsValid(Volume))
		{
			float WaterPlaneZ = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z;
			HumanoidCharacterAttributesComponent->SetSuffocating(HeadPosition.Z < WaterPlaneZ);
		}
	}
}

// TODO GAS GAS GAS I wanna stick to the gas
void ABaseHumanoidCharacter::OnOutOfStamina(bool bOutOfStamina)
{
	if (bOutOfStamina)
	{
		if (HumanoidMovementComponent->IsSprinting())
		{
			StopSprinting();
			SetStrafingControlsState(false);
		}
		else if (HumanoidMovementComponent->IsWallrunning())
		{
			StopWallrunning();
		}

		OnActionStarted(ECharacterAction::OutOfStamina);
	}
	else
	{
		OnActionEnded(ECharacterAction::OutOfStamina);
		UpdateStrafingControls();
	}
	
	HumanoidMovementComponent->SetIsOutOfStamina(bOutOfStamina);
}

#pragma endregion ATTRIBUTES

#pragma region SPRINT

void ABaseHumanoidCharacter::StartRequestingSprint()
{
	bSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABaseHumanoidCharacter::StopRequestingSprint()
{
	bSprintRequested = false;
}

bool ABaseHumanoidCharacter::CanSprint() const
{
	return IsPendingMovement()
		&& !CharacterCombatComponent->IsAiming()
		&& HumanoidMovementComponent->IsMovingOnGround() && !HumanoidCharacterAttributesComponent->IsOutOfStamina()
		&& HumanoidMovementComponent->GetCurrentPosture() != EPosture::Proning;
}

void ABaseHumanoidCharacter::TryStartSprinting()
{
	bool bSprintStarted = HumanoidMovementComponent->TryStartSprint();
	if (bSprintStarted && CharacterEquipmentComponent->GetEquippedItemType() != EEquippableItemType::None)
	{
		SetStrafingControlsState(false);
	}
	
	OnSprintStart();
}

void ABaseHumanoidCharacter::StopSprinting()
{
	if (!HumanoidMovementComponent->IsSprinting())
	{
		return;
	}
	
	HumanoidMovementComponent->StopSprint();
	if (HumanoidMovementComponent->IsMovingOnGround() && CharacterEquipmentComponent->IsPreferStrafing())
	{
		SetStrafingControlsState(true);
	}
	
	OnSprintEnd();
}

void ABaseHumanoidCharacter::TryChangeSprintState()
{
	if (bSprintRequested && !HumanoidMovementComponent->IsSprinting() && CanSprint())
	{
		TryStartSprinting();
	}
	else if ((!bSprintRequested || !CanSprint()) && HumanoidMovementComponent->IsSprinting())
	{
		StopSprinting();
	}
}

#pragma endregion 

#pragma region JUMP/FALL

void ABaseHumanoidCharacter::MoveForward(float Value)
{
	Super::MoveForward(Value);
	HumanoidMovementComponent->SetForwardInput(Value);
}

void ABaseHumanoidCharacter::Jump()
{
	if (HumanoidMovementComponent->GetCurrentPosture() == EPosture::Proning)
	{
		HumanoidMovementComponent->RequestStandUp();
	}
	else if (bIsCrouched)
	{
		UnCrouch();
	}
	else if (HumanoidMovementComponent->IsClimbing())
	{
		HumanoidMovementComponent->StopClimbing(EStopClimbingMethod::JumpOff);
	}
	else if (HumanoidMovementComponent->IsZiplining())
	{
		StopZiplining();
	}
	else if (HumanoidMovementComponent->IsWallrunning())
	{
		HumanoidMovementComponent->JumpOffWall();
	}
	else if (CanStartAction(ECharacterAction::Jump))
	{
		if (HumanoidMovementComponent->IsSliding())
			StopSliding();

		Super::Jump();
	}
}

void ABaseHumanoidCharacter::OnJumped_Implementation()
{
	HumanoidCharacterAttributesComponent->OnJumped();
	OnActionStarted(ECharacterAction::Jump);
	OnActionEnded(ECharacterAction::Jump);
}

bool ABaseHumanoidCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation()
		&& !HumanoidCharacterAttributesComponent->IsOutOfStamina()
		&& !HumanoidMovementComponent->IsMantling()
		&& !HumanoidMovementComponent->IsProning();
}

void ABaseHumanoidCharacter::Falling()
{
	if (HumanoidMovementComponent->IsSprinting())
	{
		StopSprinting();
	}
}

void ABaseHumanoidCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	HumanoidMovementComponent->bNotifyApex = 1;
	float VerticalSpeed = HumanoidMovementComponent->Velocity.Z;
	if (Hit.bBlockingHit && -VerticalSpeed > HumanoidCharacterSettings->HardLandVelocityZ)
	{
		const float SurfaceNormalZToStopSliding = 0.9f;
		if (HumanoidMovementComponent->IsSliding() && Hit.ImpactNormal.Z > SurfaceNormalZToStopSliding)
		{
			StopSliding(true);
		}

		FMontagePlayResult MontagePlayResult = PlayHardLandMontage();
		if (MontagePlayResult.bStarted)
		{
			HumanoidMovementComponent->Velocity = FVector::ZeroVector;
			SetInputDisabled(true, MontagePlayResult.bDisableCameraRotation);
			MontagePlayResult.AnimInstance->Montage_SetEndDelegate(MontageEndedUnlockControlsEvent, MontagePlayResult.AnimMontage);	
		}
	}

	if (IsValid(HumanoidCharacterSettings->FallDamageCurve))
	{
		float FallDamage = HumanoidCharacterSettings->FallDamageCurve->GetFloatValue(-VerticalSpeed);
		if (FallDamage > 0.f)
			TakeDamage(FallDamage, GetEnvironmentDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

void ABaseHumanoidCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	FallApexWorldZ = GetActorLocation().Z;
}

FMontagePlayResult ABaseHumanoidCharacter::PlayHardLandMontage()
{
	return PlayHardLandMontage(GetMesh()->GetAnimInstance(), HumanoidCharacterSettings->HardLandMontageTP);
}

FMontagePlayResult ABaseHumanoidCharacter::PlayHardLandMontage(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage) const
{
	FMontagePlayResult MontagePlayResult;
	MontagePlayResult.bStarted = false;
	if (IsValid(AnimInstance))
	{
		if (IsValid(AnimMontage))
		{
			MontagePlayResult.bStarted = AnimInstance->Montage_Play(AnimMontage, 1, EMontagePlayReturnType::Duration, 0.f) > 0;
			if (MontagePlayResult.bStarted)
			{
				MontagePlayResult.AnimInstance = AnimInstance;
				MontagePlayResult.AnimMontage = AnimMontage;
			}
		}
	}

	return MontagePlayResult;
}

#pragma endregion JUMP/FALL

#pragma region INTERACTION

void ABaseHumanoidCharacter::Interact()
{
	if (bInteracting)
	{
		TryStopInteracting();
	}
	else
	{
		TryStartInteracting();
	}
}

void ABaseHumanoidCharacter::TryStopInteracting()
{
	if (CurrentInteractable.IsValid())
	{
		switch (CurrentInteractable->GetInteractionType())
		{
			case EInteractionType::Climbing:
				HumanoidMovementComponent->StopClimbing();
				break;
			case EInteractionType::Ziplining:
				break;
			default:
				return;
		}	
	}

	ResetInteraction();
}

// TODO InteractionComponent
bool ABaseHumanoidCharacter::TryStartInteracting()
{
	const AInteractiveActor* Interactable = GetPreferableInteractable();
	if (!IsValid(Interactable))
	{
		return false;
	}

	bool bInteractionStarted = false;
	switch (Interactable->GetInteractionType())
	{
		case EInteractionType::Climbing:
		{
			if (!HumanoidCharacterSettings->bCanClimb)
				break;
				
			const ALadder* Ladder = StaticCast<const ALadder*>(Interactable);
			if (Ladder->IsOnTop())
				PlayAnimMontage(Ladder->GetAttachFromTopAnimMontage());
	
			bInteractionStarted = HumanoidMovementComponent->TryStartClimbing(Ladder);
			break;
		}
		case EInteractionType::Ziplining:
		{
			if (!HumanoidCharacterSettings->bCanZipline)
				break;
				
			const AZipline* Zipline = StaticCast<const AZipline*>(Interactable);
			FZiplineParams ZiplineParams = GetZipliningParameters(Zipline);
			bInteractionStarted = HumanoidMovementComponent->TryStartZiplining(ZiplineParams);
			break;
		}
		default:
			break;
	}

	if (bInteractionStarted)
	{
		bInteracting = true;
		CurrentInteractable = Interactable;	
	}

	return bInteractionStarted;
}

void ABaseHumanoidCharacter::RegisterInteractiveActor(const AInteractiveActor* InteractiveActor)
{
	InteractiveActor->Tags;
	InteractiveActors.AddUnique(InteractiveActor);
}

void ABaseHumanoidCharacter::UnregisterInteractiveActor(const AInteractiveActor* InteractiveActor)
{
	if (InteractiveActor != CurrentInteractable)
		InteractiveActors.RemoveSingleSwap(InteractiveActor);
}

const AInteractiveActor* ABaseHumanoidCharacter::GetPreferableInteractable()
{
	auto InteractablesCount = InteractiveActors.Num();	
	if (InteractablesCount <= 0)
		return nullptr;

	const AInteractiveActor* Interactable = nullptr;
	while (InteractablesCount > 0)
	{
		Interactable = InteractiveActors[0];
		if (IsValid(Interactable))
			break;
		
		InteractiveActors.RemoveSingleSwap(CurrentInteractable.Get());
		Interactable = nullptr;
		InteractablesCount--;
	}
	
	return Interactable;
}

void ABaseHumanoidCharacter::ResetInteraction()
{
	bInteracting = false;
	CurrentInteractable = nullptr;
}

#pragma endregion INTERACTION

#pragma region CROUCH/PRONE

void ABaseHumanoidCharacter::ToggleCrouchState()
{
	if (!bIsCrouched && !HumanoidMovementComponent->IsSliding() && HumanoidCharacterSettings->bCanCrouch && CanStartAction(ECharacterAction::Crouch))
	{
		HumanoidMovementComponent->RequestCrouch();
	}
}

void ABaseHumanoidCharacter::ToggleProneState()
{
	if (bIsCrouched && HumanoidCharacterSettings->bCanCrawl && CanStartAction(ECharacterAction::Crawl))
	{
		HumanoidMovementComponent->RequestProne();
	}
}

void ABaseHumanoidCharacter::OnStartCrouchOrProne(float HalfHeightAdjust)
{
	UpdateStrafingControls();
	ChangeMeshOffset(HalfHeightAdjust);
}

void ABaseHumanoidCharacter::OnEndCrouchOrProne(float HalfHeightAdjust)
{
	OnActionEnded(ECharacterAction::Crawl);
	OnActionEnded(ECharacterAction::Crouch);
	UpdateStrafingControls();
	ChangeMeshOffset(-HalfHeightAdjust);
}

#pragma endregion CROUCH/PRONE

#pragma region ZIPLINE

void ABaseHumanoidCharacter::OnZiplineObstacleHit(FHitResult Hit)
{
	if (Hit.bBlockingHit)
	{
		StopZiplining();
	}
}

void ABaseHumanoidCharacter::StopZiplining()
{
	HumanoidMovementComponent->StopZiplining();
	ResetInteraction();
}

FZiplineParams ABaseHumanoidCharacter::GetZipliningParameters(const AZipline* Zipline) const
{
	FZiplineParams ZiplineParams;
	ZiplineParams.Friction = Zipline->GetCableFriction();
	ZiplineParams.ZiplineNormalizedDirection = Zipline->GetZiplineDirection();
	ZiplineParams.DeclinationAngle = Zipline->GetDeclinationAngle();
	const FVector TopPoleLocation = Zipline->GetTopPoleWorldLocation();
	const FVector BottomPoleLocation = Zipline->GetBottomPoleWorldLocation();
	const FVector TopPoleProjection = FVector(TopPoleLocation.X, TopPoleLocation.Y, BottomPoleLocation.Z); 	
	ZiplineParams.MovementPlane = FPlane(TopPoleLocation,BottomPoleLocation, TopPoleProjection);
				
	const float ZiplineDistance = (TopPoleLocation - BottomPoleLocation).Size();
	FVector ActorLocation = GetActorLocation();
	FVector ProjectedVelocity = FVector::VectorPlaneProject(GetVelocity(), ZiplineParams.MovementPlane.GetNormal());
	ZiplineParams.CurrentSpeed = ProjectedVelocity.Size();
	
	ZiplineParams.DeclinationAngleSin = FMath::Sin(FMath::DegreesToRadians(ZiplineParams.DeclinationAngle));
	ZiplineParams.DeclinationAngleCos = FMath::Cos(FMath::DegreesToRadians(ZiplineParams.DeclinationAngle));
	
	FVector SocketOffset = GetMesh()->GetSocketTransform(HumanoidCharacterSettings->ZiplineHandPositionSocketName,RTS_Actor).GetLocation();
	SocketOffset = ZiplineParams.ZiplineNormalizedDirection.ToOrientationRotator().RotateVector(SocketOffset);
	
	const float CharacterToZiplineDistance = (TopPoleLocation - ActorLocation).Size();
	FVector CableAttachmentLocation = TopPoleLocation + (BottomPoleLocation - TopPoleLocation) * CharacterToZiplineDistance / ZiplineDistance;
	float RotationHeightDeltaAngle = (180.f - ZiplineParams.DeclinationAngle) * 0.5f;
	const UCapsuleComponent* CharacterCapsule = GetCapsuleComponent();

	const float Height = CharacterCapsule->GetScaledCapsuleHalfHeight() + CharacterCapsule->GetScaledCapsuleRadius(); 
	const float HalfHeightAdjustment = Height * ZiplineParams.DeclinationAngleSin * (1.f / FMath::Tan(FMath::DegreesToRadians(RotationHeightDeltaAngle)));
	// const float HalfHeightAdjustment = (CharacterCapsule->GetScaledCapsuleHalfHeight())
	// 	* ZiplineParams.DeclinationAngleSin * (1.f / FMath::Tan(FMath::DegreesToRadians(RotationHeightDeltaAngle)));
	
	// ZiplineParams.CorrectedActorLocation = CableAttachmentLocation - (SocketOffset + HalfHeightAdjustment) * FVector::UpVector;
	ZiplineParams.CorrectedActorLocation = CableAttachmentLocation - SocketOffset;
	// ZiplineParams.CorrectedActorLocation = CableAttachmentLocation - SocketOffset - HalfHeightAdjustment * FVector::UpVector;
	ZiplineParams.AdjustedHandPosition = CableAttachmentLocation;
	return ZiplineParams;
}

#pragma endregion ZIPLINE

#pragma region MANTLE

void ABaseHumanoidCharacter::Mantle(bool bForce)
{
	if (!CanMantle() && !bForce)
		return;
	
	FLedgeDescriptor LedgeDescriptor;
	// TODO refactor to MantlingComponent one day
	if (LedgeDetectionComponent->DetectLedge(LedgeDescriptor))
	{
		const auto ActorTransform = GetActorTransform();
		const auto MantleSettings = GetMantlingSettings(LedgeDescriptor.MantlingHeight);
		FMantlingMovementParameters MantleParams(LedgeDescriptor, ActorTransform, MantleSettings);
		if (bIsCrouched)
		{
			const float DefaultHalfHeight = GetDefaultHalfHeight() * GetActorScale().Z;
			MantleParams.InitialLocation.Z += DefaultHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}

		if (GetLocalRole() > ROLE_SimulatedProxy)
		{
			bool bMantleStarted = HumanoidMovementComponent->TryStartMantle(MantleParams);
			if (!bMantleStarted)
				return;
		
			if (bIsCrouched)
				UnCrouch();
		}
		
		PlayMantleMontage(MantleSettings, MantleParams.StartTime);
	}
}

void ABaseHumanoidCharacter::PlayMantleMontage(const FMantlingSettings& MantleSettings, float StartTime)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(MantleSettings.MantleMontageTP))
	{
		AnimInstance->Montage_Play(MantleSettings.MantleMontageTP, 1, EMontagePlayReturnType::Duration, StartTime);
	}
}

const FMantlingSettings& ABaseHumanoidCharacter::GetMantlingSettings(float Height) const
{
	return Height <= HumanoidCharacterSettings->MantleLowMaxHeight
		? HumanoidCharacterSettings->MantleLowSettings
		: HumanoidCharacterSettings->MantleHighSettings;
}

bool ABaseHumanoidCharacter::CanMantle() const
{
	return HumanoidCharacterSettings->bCanMantle
		&& !HumanoidMovementComponent->IsMantling()
		&& (HumanoidMovementComponent->GetCurrentPosture() == EPosture::Standing
			|| HumanoidMovementComponent->GetCurrentPosture() == EPosture::Crouching)
		&& !HumanoidMovementComponent->IsWallrunning()
		&& IsValid(HumanoidCharacterSettings->MantleHighSettings.MantleCurve)
		&& IsValid(HumanoidCharacterSettings->MantleLowSettings.MantleCurve);
}

void ABaseHumanoidCharacter::OnRep_Mantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy && !bWasMantling && bMantlingRep)
	{
		Mantle(true);
	}
}

#pragma endregion MANTLE

#pragma region CLIMB

void ABaseHumanoidCharacter::OnClimbableTopReached()
{
	Mantle(true);
	InteractiveActors.RemoveSingleSwap(CurrentInteractable.Get());
	ResetInteraction();
}

void ABaseHumanoidCharacter::OnStoppedClimbing(const AInteractiveActor* Interactable)
{
	if (bInteracting && Interactable == CurrentInteractable)
	{
		// TODO refactor so that these fields are updated in a single method
		ResetInteraction();
	}
}

#pragma endregion CLIMB

#pragma region WALLRUN

void ABaseHumanoidCharacter::StartRequestingWallrun()
{	
	if (CanAttemptWallrun())
	{
		HumanoidMovementComponent->RequestWallrunning();
	}
}

void ABaseHumanoidCharacter::StopRequestingWallrun()
{
	HumanoidMovementComponent->StopWallrunning(false);
}

bool ABaseHumanoidCharacter::CanAttemptWallrun() const
{
	return HumanoidCharacterSettings->bCanWallrun
		&& IsPendingMovement()
		&& !HumanoidCharacterAttributesComponent->IsOutOfStamina()
		&& (HumanoidMovementComponent->IsMovingOnGround() || HumanoidMovementComponent->IsFalling())
		&& HumanoidMovementComponent->GetCurrentPosture() == EPosture::Standing;
}

void ABaseHumanoidCharacter::StopWallrunning()
{
	HumanoidMovementComponent->StopWallrunning(false);
}

#pragma endregion WALLRUN

#pragma region SLIDE

void ABaseHumanoidCharacter::TryStartSliding()
{
	if (CanSlide())
	{
		bool bSlideStarted = HumanoidMovementComponent->TryStartSliding();
		if (bSlideStarted)
		{
			StopSprinting();
		}
	}
}

void ABaseHumanoidCharacter::StopSliding(bool bForce)
{
	if (bForce || HumanoidMovementComponent->IsSliding())
	{
		HumanoidMovementComponent->StopSliding();
	}
}

void ABaseHumanoidCharacter::OnSlidingStateChangedEvent(bool bSliding, float HalfHeightAdjust)
{
	ChangeMeshOffset(bSliding ? HalfHeightAdjust : -HalfHeightAdjust);
}

#pragma endregion SLIDE

void ABaseHumanoidCharacter::HandleCancellingActions(ECharacterAction ActionToInterrupt)
{
	Super::HandleCancellingActions(ActionToInterrupt);
	switch (ActionToInterrupt)
	{
		case ECharacterAction::Sprint:
			StopSprinting();
			break;
		case ECharacterAction::Climb:
		case ECharacterAction::Zipline:
			TryStopInteracting();
			break;
		case ECharacterAction::Wallrun:
			HumanoidMovementComponent->StopWallrunning(false);
			break;
		case ECharacterAction::Crawl:
		case ECharacterAction::Crouch:
			HumanoidMovementComponent->UnCrouch();
			break;
		case ECharacterAction::Slide:
			HumanoidMovementComponent->StopSliding();
			break;
		case ECharacterAction::CarryObject:
			CarryingComponent->StopCarrying();
			break;
		default:
			break;
	}
}

bool ABaseHumanoidCharacter::IsPreferStrafing() const
{
	return Super::IsPreferStrafing()
		&& (HumanoidMovementComponent->GetCurrentPosture() == EPosture::Standing || HumanoidMovementComponent->GetCurrentPosture() == EPosture::Crouching)
		&& !HumanoidMovementComponent->IsSprinting()
		&& !HumanoidMovementComponent->IsZiplining();
}

bool ABaseHumanoidCharacter::CanReload() const
{
	return Super::CanReload() || HumanoidMovementComponent->IsProning();
}

bool ABaseHumanoidCharacter::CanShoot() const
{
	return Super::CanShoot() && !HumanoidMovementComponent->IsClimbing()
		&& !HumanoidMovementComponent->IsZiplining() && !HumanoidMovementComponent->IsWallrunning()
		&& !HumanoidMovementComponent->IsMantling();
}

void ABaseHumanoidCharacter::OnAimStateChanged(bool bAiming, ARangeWeaponItem* Weapon)
{
	HumanoidMovementComponent->SetIsAiming(bAiming);
	if (bAiming && IsValid(Weapon))
	{
		OnAimingStart(Weapon->GetAimFOV(), Weapon->GetAimTurnModifier(), Weapon->GetAimLookUpModifier());
	}
	else
	{
		OnAimingEnd();
	}
}

#pragma endregion WEAPONS

#pragma region MOVEMENT

void ABaseHumanoidCharacter::SetInputDisabled(bool bDisabledMovement, bool bDisabledCamera)
{
	if (IsValid(Controller))
	{
		// Moved these 2 lines to player controller
		// Controller->SetIgnoreLookInput(bDisabledCamera);
		// Controller->SetIgnoreMoveInput(bDisabledMovement);
		if (InputLockedEvent.IsBound())
			InputLockedEvent.Broadcast(bDisabledMovement);
		if (bDisabledMovement)
		{
			CurrentInputForward = 0.f;
			CurrentInputRight = 0.f;
		}
	}
}

void ABaseHumanoidCharacter::OnMontageEndedUnlockControls(UAnimMontage* AnimMontage, bool bInterrupted)
{
	SetInputDisabled(false, false);
	UpdateStrafingControls();
}

void ABaseHumanoidCharacter::OnMantleEnded()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
}

void ABaseHumanoidCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	switch (PrevMovementMode)
	{
		case EMovementMode::MOVE_Swimming:
			HumanoidCharacterAttributesComponent->SetSuffocating(false);
			break;
		default:
			break;
	}

	UpdateStrafingControls();
}

#pragma endregion MOVEMENT

#pragma region REPLICATION

void ABaseHumanoidCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParamsSimulatedOnly;
	RepParamsSimulatedOnly.Condition = ELifetimeCondition::COND_SimulatedOnly;
	DOREPLIFETIME_WITH_PARAMS(ABaseHumanoidCharacter, bSprintingRep, RepParamsSimulatedOnly)
	DOREPLIFETIME_WITH_PARAMS(ABaseHumanoidCharacter, bMantlingRep, RepParamsSimulatedOnly)
}

#pragma endregion REPLICATION

void ABaseHumanoidCharacter::ChangeMeshOffset(float HalfHeightAdjust)
{
	RecalculateBaseEyeHeight();
	FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z += HalfHeightAdjust;
	BaseTranslationOffset.Z = MeshRelativeLocation.Z;
}

void ABaseHumanoidCharacter::StartSoaking()
{
	SoakingComponent->StartSoaking();
}

void ABaseHumanoidCharacter::StopSoaking()
{
	SoakingComponent->StopSoaking();
}

bool ABaseHumanoidCharacter::IsConsumingStamina() const
{
	return Super::IsConsumingStamina() || HumanoidMovementComponent->IsSprinting();
}

void ABaseHumanoidCharacter::OnDesiredRotationReached()
{
	UpdateStrafingControls();
}
