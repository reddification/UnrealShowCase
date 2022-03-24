#include "HumanoidCharacterMovementComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "CommonConstants.h"
#include "DebugSubsystem.h"
#include "Actors/Interactive/Environment/Ladder.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Character/SoakingComponent.h"
#include "Data/DataAssets/Components/CarryingSettings.h"
#include "Data/DataAssets/Components/Movement/ClimbingSettings.h"
#include "Data/DataAssets/Components/Movement/MovementSettings.h"
#include "Data/DataAssets/Components/Movement/SlideSettings.h"
#include "Data/DataAssets/Components/Movement/WallrunSettings.h"
#include "Data/DataAssets/Components/Movement/ZiplineSettings.h"
#include "Data/Movement/GCMovementMode.h"
#include "Data/Movement/SlideData.h"
#include "Data/Movement/WakeUpParams.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/TraceUtils.h"

void UHumanoidCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(IsValid(MovementSettings), TEXT("Movement Settings Data Asset must be set"));
	DefaultWalkSpeed = MaxWalkSpeed;
	CurrentAimSpeed = MovementSettings->DefaultAimingSpeed;
	GCCharacter = Cast<ABaseHumanoidCharacter>(CharacterOwner);
	if (IsValid(WallrunSettings))
		CharacterOwner->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UHumanoidCharacterMovementComponent::OnPlayerCapsuleHit);
	
	InitPostureHalfHeights();
}

float UHumanoidCharacterMovementComponent::GetMaxSpeed() const
{
	float RawSpeed = 0.f;
	if (bOutOfStamina)
		RawSpeed = MovementSettings->OutOfStaminaSpeed;
	else if (bSprinting)
		RawSpeed =  MovementSettings->SprintSpeed;
	else if (IsClimbing())
		RawSpeed =  ClimbingSettings->MaxClimbingSpeed;
	else if (CurrentPosture == EPosture::Proning)
		RawSpeed =  MovementSettings->CrawlSpeed;	
	else if (IsWallrunning())
		RawSpeed =  WallrunSettings->MaxSpeed;
	else if (IsSliding())
		RawSpeed =  SlideSettings->MaxSpeed;
	else if (bAiming)
		RawSpeed =  CurrentAimSpeed;
	else if (GCCharacter->GetCarryingComponent()->IsCarrying())
		RawSpeed =  GCCharacter->GetCarryingComponent()->GetSettings()->CarryingMovementSpeed;
	else if (bWalking)
		RawSpeed = MovementSettings->WalkingSpeed;
	else 
		RawSpeed = Super::GetMaxSpeed();
	
	float Soakness = GCCharacter->GetSoakingComponent()->GetSoakness();
	return FMath::Max(50.f, RawSpeed * (1.f - Soakness));
}

void UHumanoidCharacterMovementComponent::ToggleWalking()
{
	bWalking = !bWalking;
}

bool UHumanoidCharacterMovementComponent::TryStartSprint()
{
	if (CurrentPosture != EPosture::Standing)
	{
		if (CurrentPosture == EPosture::Crouching)
		{
			if (!TryWakeUpToState(EPosture::Standing))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	
	bSprinting = true;
	GCCharacter->OnActionStarted(ECharacterAction::Sprint);
	return true;
}

void UHumanoidCharacterMovementComponent::StopSprint()
{
	GCCharacter->OnActionEnded(ECharacterAction::Sprint);
	bSprinting = false;
}

void UHumanoidCharacterMovementComponent::SetIsOutOfStamina(bool bNewOutOfStamina)
{
	bOutOfStamina = bNewOutOfStamina;
	MaxWalkSpeed = bNewOutOfStamina ? MovementSettings->OutOfStaminaSpeed : DefaultWalkSpeed;
	if (bNewOutOfStamina && bSprinting)
	{
		StopSprint();
	}
}

void UHumanoidCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		if (bWantsToCrouch && CurrentPosture != EPosture::Crouching)
		{
			Crouch();
		}
		else if (bWantsToProne && CurrentPosture != EPosture::Proning)
		{
			Prone();
		}
		else if (!bWantsToCrouch && CurrentPosture == EPosture::Crouching)
		{
			UnCrouch();
		}
		else if (!bWantsToProne && CurrentPosture == EPosture::Proning)
		{
			UnProne();
		}
	}
}

void UHumanoidCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		if (IsCrouching())
		{
			UnCrouch();
		}
		else if (IsProning())
		{
			UnProne();
		}
		else if (PreviousMovementMode == MOVE_Custom)
		{
			if (PreviousCustomMode == (uint8)EGCMovementMode::CMOVE_Slide)
			{
				ResetFromSliding();
			}
			else if (PreviousCustomMode == (uint8)EGCMovementMode::CMOVE_WallRun)
			{
				StopWallrunning(true);
			}
		}

		bNotifyApex = true;
		GCCharacter->OnActionStarted(ECharacterAction::Swim);
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(MovementSettings->SwimmingCapsuleRadius, MovementSettings->SwimmingCapsuleHalfHeight);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		const UCapsuleComponent* DefaultCharacterCapsule = CharacterOwner->GetClass()
			->GetDefaultObject<ACharacter>()->GetCapsuleComponent();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacterCapsule->GetUnscaledCapsuleRadius(),
			DefaultCharacterCapsule->GetUnscaledCapsuleHalfHeight(),true);
		FRotator Rotation = CharacterOwner->GetActorRotation();
		Rotation.Roll = 0;
		CharacterOwner->SetActorRotation(Rotation);
		GCCharacter->OnActionEnded(ECharacterAction::Swim);
	}
	else if (PreviousMovementMode == MOVE_Custom)
	{
		switch (PreviousCustomMode)
		{
		case EGCMovementMode::CMOVE_Climbing:
			StoppedClimbing.ExecuteIfBound(CurrentClimbable);
			CurrentClimbable = nullptr;
			GCCharacter->OnActionEnded(ECharacterAction::Climb);
			break;
		default:
			break;
		}
	}
}

#pragma region CROUCH

void UHumanoidCharacterMovementComponent::RequestStandUp()
{
	bWantsToProne = false;
	bWantsToCrouch = false;
}

void UHumanoidCharacterMovementComponent::RequestCrouch()
{
	bWantsToProne = false;
	bWantsToCrouch = true;
}

void UHumanoidCharacterMovementComponent::Crouch(bool bClientSimulation)
{
	if (!bClientSimulation && !CanCrouchInCurrentState() || !IsMovingOnGround() || IsSprinting() || !GCCharacter->CanStartAction(ECharacterAction::Crouch))
	{
		return;
	}
	
	const auto DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	float ScaledHalfHeightAdjust = 0.f;
	if (TryCrouchOrProne(CrouchedHalfHeight, DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), ScaledHalfHeightAdjust))
	{
		CurrentPosture = EPosture::Crouching;
		// IDK if I can get rid of this since i'm using postures now
		CharacterOwner->bIsCrouched = true;
		CrouchedOrProned.ExecuteIfBound(ScaledHalfHeightAdjust);
		GCCharacter->OnActionStarted(ECharacterAction::Crouch);
	}
	else
	{
		bWantsToCrouch = false;
	}
}

void UHumanoidCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	bool bWokeUp = TryWakeUpToState(EPosture::Standing);
	if (bWokeUp)
	{
		GCCharacter->OnActionEnded(ECharacterAction::Crouch);
	}
}

void UHumanoidCharacterMovementComponent::ResetAimingSpeed()
{
	if (IsValid(MovementSettings))
	{
		CurrentAimSpeed = MovementSettings->DefaultAimingSpeed;
	}
}

#pragma endregion CROUCH

#pragma region PRONE

void UHumanoidCharacterMovementComponent::Prone()
{
	if (!CanProne() || !GCCharacter->CanStartAction(ECharacterAction::Crawl))
	{
		bWantsToProne = false;
		return;
	}

	float ScaledHalfHeightAdjust = 0.f;
	if (TryCrouchOrProne(MovementSettings->ProneCapsuleHalfHeight, MovementSettings->ProneCapsuleRadius, ScaledHalfHeightAdjust))
	{
		CurrentPosture = EPosture::Proning;
		CharacterOwner->bIsCrouched = false;
		CrouchedOrProned.ExecuteIfBound(ScaledHalfHeightAdjust);
		GCCharacter->OnActionStarted(ECharacterAction::Crawl);
	}
	else
	{
		bWantsToProne = false;
	}
}

void UHumanoidCharacterMovementComponent::UnProne()
{
	bool bWokeUp = TryWakeUpToState(EPosture::Standing);
	if (bWokeUp)
	{
		GCCharacter->OnActionEnded(ECharacterAction::Crawl);
	}
}

// TODO refactor with bClientSimulation
bool UHumanoidCharacterMovementComponent::TryCrouchOrProne(float NewCapsuleHalfHeight, float NewCapsuleRadius, float& ScaledHalfHeightAdjust)
{
	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == NewCapsuleHalfHeight)
	{
		CrouchedOrProned.ExecuteIfBound(0);
		return true;
	}

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedNewHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, NewCapsuleHalfHeight);

	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedNewHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	// when going to crouch state from proned
	if (ClampedNewHalfHeight > OldUnscaledHalfHeight)
	{
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const bool IsBlocked = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f,0.f,ScaledHalfHeightAdjust),
			FQuat::Identity,UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

		if(IsBlocked)
		{
			return false;
		}
	}

	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(NewCapsuleRadius, ClampedNewHalfHeight);
	
	UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust),
		UpdatedComponent->GetComponentQuat(), true, nullptr,
		EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	
	bForceNextFloorCheck = true;
	AdjustProxyCapsuleSize();
	return true;
}

void UHumanoidCharacterMovementComponent::FillWakeUpParams(FWakeUpParams& WakeUpParams) const
{
	const auto CapsuleComponent = CharacterOwner->GetCapsuleComponent();
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	WakeUpParams.DefaultCharacter = DefaultCharacter;
	WakeUpParams.CapsuleComponent = CapsuleComponent;
	WakeUpParams.ComponentScale = CapsuleComponent->GetShapeScale();
	WakeUpParams.SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	WakeUpParams.PawnLocation = UpdatedComponent->GetComponentLocation();
	WakeUpParams.CollisionQueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
	InitCollisionParams(WakeUpParams.CollisionQueryParams, WakeUpParams.ResponseParam);
	WakeUpParams.CollisionChannel = UpdatedComponent->GetCollisionObjectType();
}

bool UHumanoidCharacterMovementComponent::TryWakeUpToState(EPosture DesiredPosture, bool bClientSimulation)
{
	float HalfHeightAdjust = 0.f;
	EPosture InitialPosture = CurrentPosture;
	if (CurrentPosture != DesiredPosture)
	{
		FWakeUpParams WakeUpParams;
		FillWakeUpParams(WakeUpParams);

		const float CurrentUnscaledHalfHeight = WakeUpParams.CapsuleComponent->GetUnscaledCapsuleHalfHeight();
		const float DesiredUnscaledHalfHeight = PostureCapsuleHalfHeights[DesiredPosture];
		for (const auto& heights : PostureCapsuleHalfHeights)
		{
			if (heights.Value > DesiredUnscaledHalfHeight)
			{
				continue;
			}

			// can't wake up to a collision shape smaller than current
			if (heights.Value < CurrentUnscaledHalfHeight)
			{
				break;
			}
			
			if (TryWakeUp(heights.Value, WakeUpParams, HalfHeightAdjust, bClientSimulation))
			{
				CurrentPosture = heights.Key;
				break;
			}
		}
	}

	// TODO get rid of CharacterOwner->bIsCrouched completely?
	switch (CurrentPosture)
	{
		case EPosture::Crouching:
			bWantsToCrouch = true;
			CharacterOwner->bIsCrouched = true;
			bWantsToProne = false;
			break;
		case EPosture::Proning:
			bWantsToCrouch = false;
			CharacterOwner->bIsCrouched = false;
			bWantsToProne = true;
			break;
		case EPosture::Standing:
			bWantsToProne = false;
			CharacterOwner->bIsCrouched = false;
			bWantsToCrouch = false;
			break;
		default:
			break;
	}

	bool bSuccess = CurrentPosture != InitialPosture;
	if (bSuccess)
		WokeUp.ExecuteIfBound(HalfHeightAdjust);
	
	return bSuccess;
}

bool UHumanoidCharacterMovementComponent::TryWakeUp(float DesiredUnscaledHalfHeight, const FWakeUpParams& WakeUpParams, float& ScaledHalfHeightAdjust,
	bool bClientSimulation)
{
	const float CurrentUnscaledHalfHeight = WakeUpParams.CapsuleComponent->GetUnscaledCapsuleHalfHeight();
	if(CurrentUnscaledHalfHeight == DesiredUnscaledHalfHeight)
	{
		return true;
	}
	
	const float CurrentHalfHeight = WakeUpParams.CapsuleComponent->GetScaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DesiredUnscaledHalfHeight - CurrentUnscaledHalfHeight;
	ScaledHalfHeightAdjust = HalfHeightAdjust * WakeUpParams.ComponentScale;
	const UWorld* MyWorld = GetWorld();

	if (!bClientSimulation)
	{
		const FCollisionShape DesiredCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom,
			-WakeUpParams.SweepInflation - ScaledHalfHeightAdjust);
		FVector DesiredLocation = WakeUpParams.PawnLocation + FVector(0.f, 0.f,DesiredCapsuleShape.GetCapsuleHalfHeight() - CurrentHalfHeight);
		bool bCantWakeUp = MyWorld->OverlapBlockingTestByChannel(DesiredLocation, FQuat::Identity, WakeUpParams.CollisionChannel,
			DesiredCapsuleShape, WakeUpParams.CollisionQueryParams, WakeUpParams.ResponseParam);

		if (bCantWakeUp)
		{
			//looks redundant, was in ACharacter::UnCrouch so why not
			if (true || IsMovingOnGround())
			{
				// Something might be just barely overhead, try moving down closer to the floor to avoid it.
				const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
				if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					DesiredLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
					bCantWakeUp = MyWorld->OverlapBlockingTestByChannel(DesiredLocation, FQuat::Identity,
						WakeUpParams.CollisionChannel, DesiredCapsuleShape, WakeUpParams.CollisionQueryParams, WakeUpParams.ResponseParam);
				}
			}				
		}

		if (bCantWakeUp)
		{
			return false;
		}

		UpdatedComponent->MoveComponent(DesiredLocation - WakeUpParams.PawnLocation, UpdatedComponent->GetComponentQuat(),
		false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		bForceNextFloorCheck = true;
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(WakeUpParams.DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(),
		DesiredUnscaledHalfHeight, true);

	AdjustProxyCapsuleSize();
	return true;
}

bool UHumanoidCharacterMovementComponent::CanProne()
{
	return (IsMovingOnGround() || IsSliding()) && !UpdatedComponent->IsSimulatingPhysics(); 
}

void UHumanoidCharacterMovementComponent::RequestProne()
{
	bWantsToProne = true;
	bWantsToCrouch = false;
}

#pragma endregion PRONE

#pragma region MANTLE

bool UHumanoidCharacterMovementComponent::TryStartMantle(const FMantlingMovementParameters& NewMantlingParameters)
{
	if (IsInCustomMovementMode(EGCMovementMode::CMOVE_Mantling) || !GCCharacter->CanStartAction(ECharacterAction::Mantle))
		return false;
	
	this->MantlingParameters = NewMantlingParameters;
	Velocity = FVector::ZeroVector;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)EGCMovementMode::CMOVE_Mantling);
	CharacterOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetWorld()->GetTimerManager().SetTimer(MantlingTimerHandle, this,
    	&UHumanoidCharacterMovementComponent::EndMantle, MantlingParameters.Duration);
	GCCharacter->OnActionStarted(ECharacterAction::Mantle);
	return true;
}

void UHumanoidCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
	CharacterOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GCCharacter->OnActionEnded(ECharacterAction::Mantle);
}

bool UHumanoidCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom
		&& CustomMovementMode == (uint8)EGCMovementMode::CMOVE_Mantling;
}

void UHumanoidCharacterMovementComponent::PhysCustomMantling(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimerHandle) + MantlingParameters.StartTime;
	FVector CurveValue = MantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	float PositionAlpha = CurveValue.X;
	float XYCorrectionAlpha = CurveValue.Y;
	float ZCorrectionAlpha = CurveValue.Z;
	FVector CorrectedInitialLocation = FMath::Lerp(MantlingParameters.InitialLocation, MantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(MantlingParameters.InitialLocation.Z, MantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);
	FVector MantleTagetDeltaLocation = IsValid(MantlingParameters.MantleTarget)
		? MantlingParameters.MantleTarget->GetActorLocation() - MantlingParameters.InitialTargetLocation
		: FVector::ZeroVector;
	FVector NextLocation = FMath::Lerp(CorrectedInitialLocation, MantlingParameters.TargetLocation + MantleTagetDeltaLocation, PositionAlpha);
	FRotator NextRotation = FMath::Lerp(MantlingParameters.InitialRotation, MantlingParameters.TargetRotation, PositionAlpha);
	FHitResult Hit;
	SafeMoveUpdatedComponent(NextLocation - GetActorLocation(), NextRotation, false, Hit);
}

#pragma endregion MANTLING

#pragma region CLIMB

float UHumanoidCharacterMovementComponent::GetActorToClimbableProjection(const ALadder* Ladder,
	const FVector& ActorLocation) const
{
	FVector LadderUpVector = Ladder->GetActorUpVector();
	FVector LadderToCharacterVector = ActorLocation - Ladder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterVector);
}

bool UHumanoidCharacterMovementComponent::TryStartClimbing(const ALadder* Ladder)
{
	if (!IsValid(ClimbingSettings) || IsClimbing() || !GCCharacter->CanStartAction(ECharacterAction::Climb))
		return false;

	CurrentClimbable = Ladder;
	FVector NewCharacterLocation;

	if (Ladder->IsOnTop())
	{
		NewCharacterLocation = Ladder->GetAttachFromTopAnimMontageStartingLocation();		
	}
	else
	{
		float CharacterToLadderProjection = GetActorToClimbableProjection(Ladder, GetActorLocation());
		Velocity = FVector::ZeroVector;
		NewCharacterLocation = Ladder->GetActorLocation()
			+ CharacterToLadderProjection * Ladder->GetActorUpVector()
			+ Ladder->GetActorForwardVector() * ClimbingSettings->ClimbingOffset;	
	}
		
	CharacterOwner->SetActorLocation(NewCharacterLocation);

	FRotator TargetOrientationRotation = Ladder->GetActorForwardVector()
		.RotateAngleAxis(180, Ladder->GetActorUpVector()).ToOrientationRotator();
	CharacterOwner->SetActorRotation(TargetOrientationRotation);
	SetMovementMode(MOVE_Custom, (uint8)EGCMovementMode::CMOVE_Climbing);
	GCCharacter->OnActionStarted(ECharacterAction::Climb);
	return true;
}

void UHumanoidCharacterMovementComponent::StopClimbing(EStopClimbingMethod StopClimbingMethod)
{
	switch (StopClimbingMethod)
	{
		case EStopClimbingMethod::ReachingTop:
			ClimbableTopReached.ExecuteIfBound();
			break;
		case EStopClimbingMethod::ReachingBottom:
			SetMovementMode(MOVE_Falling);
			break;
		case EStopClimbingMethod::JumpOff:
			{
				FVector JumpDirection = FVector::ZeroVector;
				float RightInput = 0;
				float UpInput = 0;
				if (GCCharacter.IsValid())
				{
					RightInput = GCCharacter->GetCurrentInputRight();
					UpInput = GCCharacter->GetCurrentInputForward();
				}
				
				if (UpInput >= 0)
				{
					FVector2D YawSourceRange(-1, 1);
					FVector2D YawTargetRange(-90, 90);
					float YawAngle = FMath::GetMappedRangeValueClamped(YawSourceRange, YawTargetRange,
						RightInput);
					FVector2D PitchSourceRange(0, 1);
					FVector2D PitchTargetRange(0, 45);
					float PitchAngle = FMath::GetMappedRangeValueClamped(PitchSourceRange, PitchTargetRange,
						UpInput);
					FRotator rotator(PitchAngle, YawAngle, 0);
					if (PitchAngle != 0 || YawAngle != 0)
						JumpDirection = rotator.RotateVector(CharacterOwner->GetActorForwardVector());
					else
						JumpDirection = -CharacterOwner->GetActorForwardVector();
				}
				else
				{
					JumpDirection = -CurrentClimbable->GetActorForwardVector();
				}
				
				SetMovementMode(MOVE_Falling);
				Launch(JumpDirection * ClimbingSettings->JumpOffClimbableSpeed + FVector(0, 0, 500));
				if (CanApplyCustomRotation())
				{
					ForceTargetRotation = JumpDirection.ToOrientationRotator();
					bForceRotation = true;	
				}
				
				break;
			}
		case EStopClimbingMethod::Fall:
		default:
			SetMovementMode(MOVE_Falling);
			break;
	}

	GCCharacter->OnActionEnded(ECharacterAction::Climb);
}

bool UHumanoidCharacterMovementComponent::IsClimbing() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom
		&& CustomMovementMode == (uint8)EGCMovementMode::CMOVE_Climbing;
}

float UHumanoidCharacterMovementComponent::GetClimbingSpeedRatio() const
{
	checkf(IsValid(CurrentClimbable), TEXT("UGCBaseCharacterMovementComponent::GetClimbableSpeedRatio cannot be invoked without an active climbable"));
	FVector ClimbableUpVector = CurrentClimbable->GetActorUpVector();
	return  FVector::DotProduct(Velocity, ClimbableUpVector) / ClimbingSettings->MaxClimbingSpeed;
}

void UHumanoidCharacterMovementComponent::PhysCustomClimbing(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 2.f, false, ClimbingSettings->ClimbingBrakingDeceleration);
	FVector Delta = Velocity * DeltaTime;
	FHitResult Hit;

	if (HasAnimRootMotion())
	{
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}
	
	FVector NewPosition = GetActorLocation() + Delta;
	float NewPositionLadderProjection = GetActorToClimbableProjection(CurrentClimbable, NewPosition);
	if (NewPositionLadderProjection < ClimbingSettings->ClimbingDetachBottomOffset && Delta.Z < 0)
	{
		StopClimbing(EStopClimbingMethod::ReachingBottom);
		return;
	}

	if (NewPositionLadderProjection > CurrentClimbable->GetHeight() - ClimbingSettings->ClimbingDetachTopOffset)
	{
		StopClimbing(EStopClimbingMethod::ReachingTop);
		return;
	}
	
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

#pragma endregion CLIMBING

#pragma region ZIPLINE

bool UHumanoidCharacterMovementComponent::TryStartZiplining(const FZiplineParams& NewZiplineParams)
{
	if (!IsValid(ZiplineSettings) || IsZiplining() || !GCCharacter->CanStartAction(ECharacterAction::Zipline))
	{
		return false;		
	}
	
	this->ZiplineParams = NewZiplineParams;
	// SetDesiredRotation(NewZiplineParams.ZiplineNormalizedDirection.ToOrientationRotator());
	// bForceRotation = true;
	// ForceTargetRotation = ;
	ZiplineParams.bCorrected = false;
	SetMovementMode(MOVE_Custom, (uint8)EGCMovementMode::CMOVE_Zipline);
	GCCharacter->OnActionStarted(ECharacterAction::Zipline);
	return true;
}

void UHumanoidCharacterMovementComponent::StopZiplining()
{
	SetMovementMode(MOVE_Falling);
	GCCharacter->OnActionEnded(ECharacterAction::Zipline);
}

bool UHumanoidCharacterMovementComponent::IsZiplining() const
{
	return IsInCustomMovementMode(EGCMovementMode::CMOVE_Zipline);
}

void UHumanoidCharacterMovementComponent::PhysCustomZiplining(float DeltaTime, int32 Iterations)
{
	const float G = -GetGravityZ();
	ZiplineParams.CurrentSpeed = ZiplineParams.CurrentSpeed + DeltaTime * G *
		(ZiplineParams.DeclinationAngleSin - ZiplineParams.Friction * ZiplineParams.DeclinationAngleCos);
	float ClampedSpeed = FMath::Clamp(ZiplineParams.CurrentSpeed, ZiplineSettings->MinZiplineSpeed,
		ZiplineSettings->MaxZiplineSpeed);
#if ENABLE_DRAW_DEBUG
	bool bDebugEnabled = GetDebugSubsystem()->IsDebugCategoryEnabled(DebugCategoryZipline);
#else
	bool bDebugEnabled = false;
#endif
	// GEngine->AddOnScreenDebugMessage(3, 3, FColor::Green,
	// 	FString::Printf(TEXT("Zipline unclamped speed: %f"), ZiplineParams.CurrentSpeed));
	Velocity = ClampedSpeed * ZiplineParams.ZiplineNormalizedDirection;
	
	const FVector UncorrectedCharacterLocation = GetActorLocation();

	if (!UncorrectedCharacterLocation.Equals(ZiplineParams.CorrectedActorLocation, 0.2f))
	{
		const float CorrectionRatio = 10.f;
		const FVector CorrectionDelta = (ZiplineParams.CorrectedActorLocation - UncorrectedCharacterLocation) * CorrectionRatio;
		Velocity += CorrectionDelta;
	}
	else if (!ZiplineParams.bCorrected)
	{
		ZiplineParams.bCorrected = true;
		if (bDebugEnabled)
		{
			DrawDebugBox(GetWorld(), ZiplineParams.CorrectedActorLocation, FVector(15, 15, 15),
				FColor::Orange, false, 30, 0, 2);
		}
	}

	if (bDebugEnabled)
	{
		DrawDebugBox(GetWorld(), ZiplineParams.CorrectedActorLocation, FVector(5, 5, 5),
			FColor::Green, false, 30);
	}
	
	ZiplineParams.CorrectedActorLocation += ZiplineParams.ZiplineNormalizedDirection * ClampedSpeed * DeltaTime;
	ZiplineParams.AdjustedHandPosition += ZiplineParams.ZiplineNormalizedDirection * ClampedSpeed * DeltaTime;
	FVector Delta = Velocity * DeltaTime;
	FHitResult Hit;
	// bool bMoved = SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
	bool bMoved = SafeMoveUpdatedComponent(Delta, ZiplineParams.ZiplineNormalizedDirection.Rotation(), true, Hit);
	if (!bMoved)
	{
		ZiplineObstacleHit.ExecuteIfBound(Hit);
	}
}

#pragma endregion ZIPLINE

#pragma region WALLRUN

void UHumanoidCharacterMovementComponent::OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsSurfaceWallrunnable(Hit.ImpactNormal))
	{
		WallrunData.Progress = 0.f;
		return;
	}
	
	if (!WallrunData.bWantsToWallrun || IsWallrunning() || bOutOfStamina || !GCCharacter->CanStartAction(ECharacterAction::Wallrun))
	{
		return;
	}

	if (Velocity.Z < WallrunSettings->MinVelocityZ || Velocity.Size() < WallrunSettings->MinRequiredSpeed)
	{
		return;
	}

	const FVector Normal = Hit.ImpactNormal;
	
	ESide Side = GetWallrunSideFromNormal(Normal);
	const FVector AverageNormal = GetWallrunSurfaceNormal(Side);
	if (AverageNormal == FVector::ZeroVector)
	{
		return;
	}

	WallrunData.Side = Side;
	const float AngleCos = FVector::DotProduct(CharacterOwner->GetActorUpVector(), AverageNormal);
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(AngleCos));
	WallrunData.SurfaceRollAngle = WallrunData.GetSideModificator(Side) * (90.f-Angle);
	WallrunData.SurfaceNormal = Normal;
	WallrunData.InitialWorldZ = GetActorLocation().Z;
	WallrunBeginEvent.Broadcast(Side);
	WallrunData.ActiveHeightDynamicCurve = IsMovingOnGround()
		? WallrunSettings->WallrunHeightDynamicsFromGroundCurve
		: WallrunSettings->WallrunHeightDynamicsFromAirCurve;
	
	SetMovementMode(MOVE_Custom, (uint8)EGCMovementMode::CMOVE_WallRun);
	GCCharacter->OnActionStarted(ECharacterAction::Wallrun);
}

ESide UHumanoidCharacterMovementComponent::GetWallrunSideFromNormal(const FVector& Normal) const
{
	const float HorizontalWallrunDotProductThreshold = 0.3f;
	const float dpSurfaceNormalRightVector = FVector::DotProduct(Normal, CharacterOwner->GetActorRightVector());
	if (dpSurfaceNormalRightVector > HorizontalWallrunDotProductThreshold)
	{
		return ESide::Left;
	}
	else if (dpSurfaceNormalRightVector < -HorizontalWallrunDotProductThreshold)
	{
		return ESide::Right;
	}

	return ESide::None;
}

bool UHumanoidCharacterMovementComponent::IsSurfaceWallrunnable(const FVector& SurfaceNormal) const
{
	return SurfaceNormal.Z >= -KINDA_SMALL_NUMBER && SurfaceNormal.Z < GetWalkableFloorZ();
}

FVector UHumanoidCharacterMovementComponent::GetWallrunSurfaceNormal(const ESide& Side, const FVector& CharacterLocationDelta) const
{
	const int SideModificator = WallrunData.GetSideModificator(Side);
#if ENABLE_DRAW_DEBUG
	bool bDebugEnabled = GetDebugSubsystem()->IsDebugCategoryEnabled(DebugCategoryWallrun);
#else
	bool bDebugEnabled = false;
#endif
	
	const float CharacterScaleZ = CharacterOwner->GetActorScale().Z;
	FVector FeetPosition = GetActorLocation() + CharacterLocationDelta
		- FVector::UpVector * WallrunSettings->FeetTraceActorZOffset * CharacterScaleZ;
	const FVector DirectionVector = CharacterOwner->GetActorRightVector() * SideModificator;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Wallrunnable);
	FHitResult FeetHit;
	EDrawDebugTrace::Type TraceDebug = bDebugEnabled ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	float CapsuleRadius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	bool bFeetTouch = UKismetSystemLibrary::LineTraceSingle(this, FeetPosition,
		FeetPosition + DirectionVector * (WallrunSettings->WallDistance + CapsuleRadius), TraceType,
		false,TArray<AActor*>(), TraceDebug, FeetHit, true);
	if (!bFeetTouch)
	{
		return FVector::ZeroVector;
	}

	FHitResult HandHit;
	FVector HandPosition = GetActorLocation() + CharacterLocationDelta
		+ FVector::UpVector * WallrunSettings->HandTraceActorZOffset * CharacterScaleZ;

	// for inclined walls 
	const float HandTraceExtendFactor = 4.f;
	bool bHandTouch = UKismetSystemLibrary::LineTraceSingle(this, HandPosition,
		HandPosition + DirectionVector * (WallrunSettings->WallDistance * HandTraceExtendFactor + CapsuleRadius), TraceType,
		false,TArray<AActor*>(), TraceDebug, HandHit, true);

	if (!bHandTouch)
	{
		return FVector::ZeroVector;
	}

	return (FeetHit.Normal + HandHit.Normal).GetSafeNormal();
}

void UHumanoidCharacterMovementComponent::RequestWallrunning()
{
	WallrunData.bWantsToWallrun = true;
}

void UHumanoidCharacterMovementComponent::StopWallrunning(bool bResetTimer)
{
	WallrunData.bWantsToWallrun = false;
	if (!IsWallrunning())
	{
		GCCharacter->OnActionEnded(ECharacterAction::Wallrun);
		return;
	}
	
	if (bResetTimer)
	{
		WallrunData.Progress = 0.f;
	}
	
	WallrunEndEvent.Broadcast(WallrunData.Side);
	SetMovementMode(GetMovementMode());
	GCCharacter->OnActionEnded(ECharacterAction::Wallrun);
}

bool UHumanoidCharacterMovementComponent::IsWallrunning() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == (uint8) EGCMovementMode::CMOVE_WallRun;
}

void UHumanoidCharacterMovementComponent::JumpOffWall()
{
	// TODO make some UPROPERTYs for these constants
	FVector JumpOffDirection = WallrunData.SurfaceNormal * 750.f
		+ CharacterOwner->GetActorUpVector() * 500.f
		+ CharacterOwner->GetActorForwardVector() * 320.f;
	StopWallrunning(true);
	bForceRotation = true;
	ForceTargetRotation = JumpOffDirection.ToOrientationRotator();
	ForceTargetRotation.Roll = 0.f;
	ForceTargetRotation.Pitch = 0.f;
	Launch(JumpOffDirection);
}

void UHumanoidCharacterMovementComponent::PhysCustomWallRun(float DeltaTime, int32 iterations)
{
	const float ForwardInputThreshold = 0.2f;
	if (CurrentForwardInput < ForwardInputThreshold)
	{
		StopWallrunning(false);
		return;
	}

	const FVector ActorUpVector = CharacterOwner->GetActorUpVector();
	float VerticalOffset = 0.f;
	FVector SurfaceNormal = FVector::ZeroVector;
	float CurrentSpeed = WallrunSettings->MaxSpeed;
	const float CurrentWallrunProgress = (WallrunData.Progress + DeltaTime) / WallrunSettings->MaxTime;
	if (IsValid(WallrunData.ActiveHeightDynamicCurve))
	{
		const float CurrentWallrunDeltaHeightFactor = WallrunData.ActiveHeightDynamicCurve->GetFloatValue(CurrentWallrunProgress);
		const float CurrentZ = GetActorLocation().Z;
		const float ExpectedDeltaZ = WallrunSettings->MaxDeltaHeight * CurrentWallrunDeltaHeightFactor;
		VerticalOffset = ExpectedDeltaZ - (CurrentZ - WallrunData.InitialWorldZ);
		WallrunData.HeightCurveValue = CurrentWallrunDeltaHeightFactor;
		SurfaceNormal = GetWallrunSurfaceNormal(WallrunData.Side,ActorUpVector * VerticalOffset);
	}
	
	if (SurfaceNormal == FVector::ZeroVector)
	{
		SurfaceNormal = GetWallrunSurfaceNormal(WallrunData.Side);
		VerticalOffset = 0.f;	
	}
	
	if (SurfaceNormal == FVector::ZeroVector)
	{
		StopWallrunning(false);
		return;
	}

	if (IsValid(WallrunSettings->WallrunSpeedDynamicsCurve))
	{
		const float WallrunSpeedMultiplier = WallrunSettings->WallrunSpeedDynamicsCurve->GetFloatValue(CurrentWallrunProgress);
		CurrentSpeed = FMath::Min(CurrentSpeed, CurrentSpeed * WallrunSpeedMultiplier);
	}
	
	WallrunData.SurfaceNormal = SurfaceNormal;
	FVector NormalVelocity = FVector::ZeroVector;
	switch (WallrunData.Side)
	{
		case ESide::Left:
			NormalVelocity = FVector::CrossProduct(SurfaceNormal, ActorUpVector).GetSafeNormal();
			break;
		case ESide::Right:
			NormalVelocity = FVector::CrossProduct(ActorUpVector, SurfaceNormal).GetSafeNormal();			
			break;
		default:
			break;
	}
	
	FHitResult Hit;
	Velocity = (NormalVelocity * CurrentSpeed + ActorUpVector * VerticalOffset) * CurrentForwardInput;
	NormalVelocity = (NormalVelocity * CurrentSpeed * DeltaTime + ActorUpVector * VerticalOffset) * CurrentForwardInput;
	FRotator Rotation = NormalVelocity.ToOrientationRotator();
	Rotation.Roll = WallrunData.SurfaceRollAngle;
	Rotation.Pitch = 0.f;
	Rotation = FMath::RInterpTo(CharacterOwner->GetActorRotation(), Rotation, DeltaTime, WallrunSettings->RotationInterpSpeed);
	SafeMoveUpdatedComponent(NormalVelocity, Rotation, true, Hit);
	if (Hit.bBlockingHit)
	{
		if (!IsSurfaceWallrunnable(Hit.ImpactNormal))
		{
			StopWallrunning(true);
			return;
		}
		
		const float dpSurfaceForwardVector = FVector::DotProduct(Hit.ImpactNormal, CharacterOwner->GetActorForwardVector());
		const float SurfaceForwardVectorCosThreshold = -0.85f;
		if (dpSurfaceForwardVector < SurfaceForwardVectorCosThreshold)
		{
			StopWallrunning(false);
		}
		else
		{
			const float HitCorrectionRatio = 2.f;
			SafeMoveUpdatedComponent(NormalVelocity + Hit.ImpactNormal * HitCorrectionRatio, Rotation, false, Hit);			
		}
	}
	
	WallrunData.Progress += DeltaTime;
	if (WallrunData.Progress > WallrunSettings->MaxTime)
	{
		StopWallrunning(false);
	}
}

#pragma endregion WALLRUN

#pragma region SLIDING

bool UHumanoidCharacterMovementComponent::IsSliding() const
{
	return IsInCustomMovementMode(EGCMovementMode::CMOVE_Slide);
}

bool UHumanoidCharacterMovementComponent::TryStartSliding()
{
	if (IsSliding() || !SlideData.bCanSlide || !GCCharacter->CanStartAction(ECharacterAction::Slide))
	{
		return false;
	}
	
	const float CharacterScaleZ = CharacterOwner->GetActorScale().Z;
	float ScaledHalfHeightAdjust = 0.f;
	TryCrouchOrProne(SlideSettings->CapsuleHalfHeight * CharacterScaleZ,
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), ScaledHalfHeightAdjust);
	SlidingStateChangedEvent.ExecuteIfBound(true, ScaledHalfHeightAdjust);
	GetWorld()->GetTimerManager().SetTimer(SlideData.TimerHandle, this,
		&UHumanoidCharacterMovementComponent::StopSliding, SlideSettings->Duration);
	CurrentPosture = EPosture::Sliding;
	WallrunData.bWantsToWallrun = false;
	SlideData.Speed = Velocity.Size();
	SetMovementMode(MOVE_Custom, (uint8)EGCMovementMode::CMOVE_Slide);
	GCCharacter->OnActionStarted(ECharacterAction::Slide);
	return true;
}

void UHumanoidCharacterMovementComponent::StopSliding()
{
	if (!IsSliding())
	{
		GCCharacter->OnActionEnded(ECharacterAction::Slide);
		return;
	}

	ResetFromSliding();
	SetMovementMode(GetMovementMode());
	GCCharacter->OnActionEnded(ECharacterAction::Slide);
}

void UHumanoidCharacterMovementComponent::ResetFromSliding()
{
	TryWakeUpToState(EPosture::Standing);
	if (CurrentPosture == EPosture::Sliding)
	{
		Prone();
	}

	SlideData.FloorAnglePitch = 0.f;
	SlideData.bCanSlide = false;
	GetWorld()->GetTimerManager().ClearTimer(SlideData.TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(SlideData.CooldownTimerHandle, this,
		&UHumanoidCharacterMovementComponent::ResetSlide, SlideSettings->CooldownTime);
}

void UHumanoidCharacterMovementComponent::ResetSlide()
{
	SlideData.bCanSlide = true;
}

void UHumanoidCharacterMovementComponent::PhysCustomSliding(float DeltaTime, int32 Iterations)
{
	const float G = -GetGravityZ();
	FHitResult FloorCheckHit;
	FCollisionQueryParams FloorCheckCollisionQueryParams;
	FloorCheckCollisionQueryParams.AddIgnoredActor(CharacterOwner);
	auto CapsuleShape = CharacterOwner->GetCapsuleComponent()->GetCollisionShape();
	CapsuleShape.Capsule.Radius *= 1.3f; 
	const float TraceDepth = 3.f;
	const FVector FloorTraceStartLocation = GetActorLocation() + CharacterOwner->GetActorUpVector() * CapsuleShape.Capsule.HalfHeight;
	// const FVector FloorTraceEndLocation = FloorTraceStartLocation - CharacterOwner->GetActorUpVector() * TraceDepth;
	const FVector FloorTraceEndLocation = GetActorLocation() - CharacterOwner->GetActorUpVector() * TraceDepth;

	FQuat Rotation = CharacterOwner->GetActorQuat();
	bool bApplyGravity = !GetWorld()->SweepSingleByChannel(FloorCheckHit, FloorTraceStartLocation,
		FloorTraceEndLocation, Rotation, ECC_Visibility, CapsuleShape, FloorCheckCollisionQueryParams);
	bool bDescending = true;

	if (!bApplyGravity)
	{
		SlideData.VerticalSpeed = 0.f;
		bNotifyApex = true;
		if (FloorCheckHit.ImpactNormal != FVector::ZeroVector)
		{
			FVector ProjectedActorForwardVector = CharacterOwner->GetActorForwardVector();
			ProjectedActorForwardVector.Z = 0.f;
			ProjectedActorForwardVector = ProjectedActorForwardVector.GetSafeNormal();
			const float SurfaceToForwardDP = FVector::DotProduct(FloorCheckHit.ImpactNormal, ProjectedActorForwardVector);
			bDescending = SurfaceToForwardDP >= 0.f;
			const float FloorAngle = FMath::Abs(90.f - FMath::RadiansToDegrees(FMath::Acos(SurfaceToForwardDP)));
			SlideData.FloorAnglePitch = FloorAngle;// * 0.8f;
			SlideData.FloorAnglePitchSin = FMath::Sin(FMath::DegreesToRadians(SlideData.FloorAnglePitch));
			SlideData.FloorAnglePitchCos = FMath::Cos(FMath::DegreesToRadians(SlideData.FloorAnglePitch));

			// FVector RightVector = FVector::RightVector;
			FVector RightVector = CharacterOwner->GetActorRightVector();
			RightVector.Z = 0.f;
			RightVector = RightVector.GetSafeNormal();
			const float SurfaceToRightDP = FVector::DotProduct(FloorCheckHit.ImpactNormal, RightVector);
			const float SurfaceToUpDP = FVector::DotProduct(FloorCheckHit.ImpactNormal, FVector::UpVector);
			bool bLeftSide = SurfaceToRightDP < 0.f;
			SlideData.FloorAngleRoll = FMath::RadiansToDegrees(FMath::Acos(SurfaceToUpDP));
			if (bLeftSide)
				SlideData.FloorAngleRoll *= -1.f;
		}
	}
	else
	{
		SlideData.VerticalSpeed -= G * DeltaTime;
		if (bNotifyApex && SlideData.VerticalSpeed)
		{
			bNotifyApex = false;
			NotifyJumpApex();
		}
	}

	const float DirectionFactor = bDescending ? 1.f : -1.f;
	SlideData.Speed = SlideData.Speed
		+ DeltaTime * G * (DirectionFactor * SlideData.FloorAnglePitchSin - SlideSettings->DeccelerationRate * SlideData.FloorAnglePitchCos);
	
	if (SlideData.Speed < 0.f)
	{
		StopSliding();
		return;
	}

	auto CharacterRotation = CharacterOwner->GetActorRotation();
	FRotator NextRotator = FRotator(SlideData.FloorAnglePitch, CharacterRotation.Yaw, SlideData.FloorAngleRoll);
	// GEngine->AddOnScreenDebugMessage(200, 5, FColor::Cyan, FString::Printf(TEXT("R: [%s]"),
	// 	*NextRotator.ToString()));
	
	CharacterRotation = FMath::RInterpTo(CharacterRotation, NextRotator, DeltaTime, SlideSettings->PitchInterpolationSpeed);

	Velocity = FMath::Max(SlideData.Speed, 0.f) * CharacterRotation.Vector();
	if (bApplyGravity)
		Velocity.Z = SlideData.VerticalSpeed;
	
	FHitResult Hit;
	bool bHit = SafeMoveUpdatedComponent(Velocity * DeltaTime, CharacterRotation, true, Hit);
	if (bHit && Hit.bBlockingHit)
	{
		if (Hit.ImpactNormal.Z < GetWalkableFloorZ())
		{
			UE_LOG(LogTemp, Log, TEXT("Sliding blocking hit occured"));
			StopSliding();
		}
		else
		{
			// sometimes the character can just stuck on walkable surface so lifting him up a lil bit and moving without sweep
			SafeMoveUpdatedComponent((Velocity + FVector::UpVector * 2.5f) * DeltaTime, CharacterRotation, false, Hit);
		}
	}
}

#pragma endregion SLIDING

void UHumanoidCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
		case EGCMovementMode::CMOVE_Mantling:
			PhysCustomMantling(DeltaTime, Iterations);
			break;
		case EGCMovementMode::CMOVE_Climbing:
			PhysCustomClimbing(DeltaTime, Iterations);
			break;
		case EGCMovementMode::CMOVE_WallRun:
			PhysCustomWallRun(DeltaTime, Iterations);
			break;
		case EGCMovementMode::CMOVE_Zipline:
			PhysCustomZiplining(DeltaTime, Iterations);
			break;
		case EGCMovementMode::CMOVE_Slide:
			PhysCustomSliding(DeltaTime, Iterations);
		default:
			break;
	}
	
	Super::PhysCustom(DeltaTime, Iterations);
}

bool UHumanoidCharacterMovementComponent::IgnorePhysicsRotation()
{
	return IsClimbing() || IsWallrunning();
}

#pragma region UTILS

void UHumanoidCharacterMovementComponent::InitPostureHalfHeights()
{
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	PostureCapsuleHalfHeights.Add(EPosture::Standing, DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	PostureCapsuleHalfHeights.Add(EPosture::Crouching, CrouchedHalfHeight);
	PostureCapsuleHalfHeights.Add(EPosture::Proning, MovementSettings->ProneCapsuleHalfHeight);
	PostureCapsuleHalfHeights.ValueSort([] (const float& A, const float &B)
	{
		return A > B;
	});
}

bool UHumanoidCharacterMovementComponent::IsInCustomMovementMode(const EGCMovementMode& Mode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == (uint8)Mode;
}

UDebugSubsystem* UHumanoidCharacterMovementComponent::GetDebugSubsystem() const
{
	if (!IsValid(DebugSubsystem))
	{
		DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	}

	return DebugSubsystem;
}

EMovementMode UHumanoidCharacterMovementComponent::GetMovementMode()
{
	FHitResult CheckFloorHit;
	const FVector TraceStart = GetActorLocation() - FVector::UpVector * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float LineTraceExtend = 50.f;
	const FVector TraceEnd = TraceStart - FVector::UpVector * LineTraceExtend;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(CharacterOwner);
	bool bOnGround = GetWorld()->LineTraceSingleByChannel(CheckFloorHit, TraceStart, TraceEnd, ECC_Visibility, CollisionQueryParams);
	return bOnGround ? EMovementMode::MOVE_Walking : EMovementMode::MOVE_Falling;
}

#pragma endregion UTILS
