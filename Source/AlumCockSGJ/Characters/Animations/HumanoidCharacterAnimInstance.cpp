#include "HumanoidCharacterAnimInstance.h"

#include "AI/Data/NpcHumanoidStateSettings.h"
#include "Data/Movement/IKData.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Character/InverseKinematicsComponent.h"
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

	const UHumanoidCharacterMovementComponent* MovementComponent = HumanoidCharacter->GetHumanoidMovementComponent();
	auto AttributesComponent = HumanoidCharacter->GetBaseCharacterAttributesComponent();
	
	bCrouching = MovementComponent->IsCrouching();
	bSprinting = HumanoidCharacter->IsSprinting();
	
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
	if (MovementComponent->IsZiplining())
		ZiplineHandLocation = MovementComponent->GetZiplineParams().AdjustedHandPosition;
	
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
