#include "HumanoidCharacterAnimInstance.h"

#include "AI/Data/NpcHumanoidStateSettings.h"
#include "Data/Movement/IKData.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Character/InverseKinematicsComponent.h"
#include "Components/Combat/CharacterCombatComponent.h"
#include "Components/Movement/HumanoidCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UHumanoidCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	APawn* Pawn = TryGetPawnOwner();
	checkf(Pawn->IsA<ABaseHumanoidCharacter>(), TEXT("UGCBaseCharacterAnimInstance can be used only with an AGCBaseCharacter derivative"))
	HumanoidCharacter = StaticCast<ABaseHumanoidCharacter*>(Pawn);
}

void UHumanoidCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!HumanoidCharacter.IsValid())
	{
		return;
	}

	const UHumanoidCharacterMovementComponent* MovementComponent = HumanoidCharacter->GetGCMovementComponent();
	auto AttributesComponent = HumanoidCharacter->GetBaseCharacterAttributesComponent();
	
	bCrouching = MovementComponent->IsCrouching();
	bSprinting = MovementComponent->IsSprinting();
	bOutOfStamina = AttributesComponent->IsOutOfStamina();
	bProning = MovementComponent->IsProning();
	bSwimming = MovementComponent->IsSwimming();	
	bClimbingLadder = MovementComponent->IsClimbing();
	ClimbingRatio = bClimbingLadder ? MovementComponent->GetClimbingSpeedRatio() : 0.f;
	bWallRunning = MovementComponent->IsWallrunning();
	WallrunSide = MovementComponent->GetWallrunSide();
	bZiplining = MovementComponent->IsZiplining();
	bSliding = MovementComponent->IsSliding();
	bCarrying = HumanoidCharacter->GetCarryingComponent()->IsCarrying();
		
	const FIkData& IkData = HumanoidCharacter->GetInverseKinematicsComponent()->GetIkData();
	// constraining max foot elevation when crouching because it looks shitty with current animations
	float AdjustedRightFootElevation = bCrouching && IkData.RightFootElevation > 0.f
		? FMath::Clamp(IkData.RightFootElevation, 0.0f, 10.0f)
		: IkData.RightFootElevation;
	RightFootEffectorLocation = FVector(-AdjustedRightFootElevation, 0.f, 0.f);
	float AdjustedLeftFootElevation = bCrouching && IkData.LeftFootElevation > 0
		? FMath::Clamp(IkData.LeftFootElevation, 0.0f, 10.0f)
		: IkData.LeftFootElevation;
	LeftFootEffectorLocation = FVector(AdjustedLeftFootElevation, 0.f, 0.f); //left socket X facing upwards
	PelvisOffset = FVector(IkData.PelvisElevation, 0.f,0.f);

	const float FootMaxPitchInclination = HumanoidCharacter->GetVelocity().Size() > 50.f ? 5.f : 30.f;
	
	//I guess animation blueprint should be better aware of some skeleton intricacies so adding extra constaints here as well
	RightFootRotator = FRotator(0.f, FMath::Clamp(IkData.RightFootPitch, -FootMaxPitchInclination, 40.f), 0.f);
	LeftFootRotator = FRotator(0.f, FMath::Clamp(IkData.LeftFootPitch, -FootMaxPitchInclination, 40.f), 0.f);

	RightLegJointTargetOffset = FVector(-IkData.RightKneeOutwardExtend, 0.f, 0.f);
	// 0.75 is a hack for the default idle half-turned pose with left foot set forward
	LeftLegJointTargetOffset = FVector(IkData.LeftKneeOutwardExtend * 0.75f, 0.f, 0.f);

	TurnHeadTowards = FMath::RInterpTo(TurnHeadTowards, HumanoidCharacter->TurnHeadTowards.Rotation(), 
	DeltaSeconds, HeadRotationInterpolationSpeed);
	FRotator CurrentRotation = HumanoidCharacter->GetActorRotation();
	RotationSpeed = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, LastCharacterRotation) * (1.f / DeltaSeconds);
	LastCharacterRotation = CurrentRotation;
	Stamina = HumanoidCharacter->GetBaseCharacterAttributesComponent()->GetStaminaNormalized();
}

void UHumanoidCharacterAnimInstance::ApplyState(const UBaseNpcStateSettings* NpcState)
{
	Super::ApplyState(NpcState);
	auto HumanoidState = StaticCast<const UNpcHumanoidStateSettings*>(NpcState);
	bFleeing = HumanoidState->bFleeing;
	bBackingOff = HumanoidState->bBackingOff;
}
