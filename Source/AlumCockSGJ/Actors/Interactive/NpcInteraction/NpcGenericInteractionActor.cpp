#include "NpcGenericInteractionActor.h"

#include "AIController.h"
#include "CommonConstants.h"
#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void ANpcGenericInteractionActor::BeginPlay()
{
	Super::BeginPlay();
	for (auto i = 0; i < ActorInteractionOptions.Num(); i++)
	{
		InteractionsTimers.Emplace(FTimerHandle());
		EndTimerDelegates.Emplace(FTimerDelegate::CreateUObject(this, &ANpcGenericInteractionActor::OnInteractionFinished, i));
	}
}

int ANpcGenericInteractionActor::GetAvailableInteractionPosition(const FGameplayTag& InteractionTag) const
{
	TArray<int> AvailablePositions;
	for (auto i = 0; i < ActorInteractionOptions.Num(); i++)
		if (!InteractingCharacters[i].IsActive() && ActorInteractionOptions[i].InteractionTag == InteractionTag)
			AvailablePositions.Emplace(i);

	return AvailablePositions.Num() > 0
		? AvailablePositions[FMath::RandRange(0, AvailablePositions.Num() - 1)]
		: -1;
}

bool ANpcGenericInteractionActor::TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag)
{
	// TODO refactor to get rid of getting actual interaction tag twice
	FGameplayTag ActualInteractionTag = GetValidInteractionTag(InteractionTag);
	FCharacterInteractionParameters CharacterInteractionParameters = Character->GetInteractionParameters(ActualInteractionTag);
	if (!CharacterInteractionParameters.InteractionMontage)
		return false;

	auto InteractionMontage = CharacterInteractionParameters.InteractionMontage;
	bool bCanStartInteracting = Super::TryStartNpcInteraction_Implementation(Character, ActualInteractionTag);
	if (!bCanStartInteracting || !IsValid(InteractionMontage))
		return false;

	const int InteractionPositionIndex = GetAvailableInteractionPosition(ActualInteractionTag);
	Character->GetCapsuleComponent()->SetCollisionProfileName(ProfileNoCollisionInteraction);
	FVector RelativeCharacterLocation = ActorInteractionOptions[InteractionPositionIndex].InteractionPosition;
	FRotator Rotator = ActorInteractionOptions[InteractionPositionIndex].bUseCustomRotation
		? ActorInteractionOptions[InteractionPositionIndex].InteractionRotation
		: GetActorRotation();
	// Character->SetActorRotation(Rotator);
	Character->SetDesiredRotation(Rotator);
	FVector ActorInteractionLocation = ActorInteractionOptions[InteractionPositionIndex].bUseCustomRotation
		? GetActorLocation() + RelativeCharacterLocation
		: GetActorLocation() + Rotator.RotateVector(RelativeCharacterLocation);
	Character->SetActorLocation(ActorInteractionLocation);
	// Character->SetActorLocation();
	// TODO move smoothly to interaction location instead of snapping
	// Cast<AAIController>(Character->GetController())->MoveToLocation(GetActorLocation() + Rotator.RotateVector(RelativeCharacterLocation));
	Character->PlayAnimMontage(InteractionMontage);
	
	FRunningInteractionData NewInteraction(Character, InteractionMontage);
	BeginInteraction(InteractionPositionIndex, NewInteraction);
	Character->GetCharacterMovement()->SetAvoidanceEnabled(false);
	// TODO disable/re-enable CMC tick?
	return true;
}

FNpcInteractionStopResult ANpcGenericInteractionActor::StopNpcInteraction_Implementation(
	ABaseCharacter* Character, bool bInterupted)
{
	if (!IsValid(Character))
		return FNpcInteractionStopResult();
	
	int CurrentInteractionIndex = GetInteractionPosition(Character);
	if (CurrentInteractionIndex < 0)
		return FNpcInteractionStopResult();

	auto& RunningInteraction = InteractingCharacters[CurrentInteractionIndex];
	auto EndSection = RunningInteraction.Montage->GetSectionIndex(MontageSectionEndLoop);
	float StandUpDuration = EndSection != INDEX_NONE ? RunningInteraction.Montage->GetSectionLength(EndSection) : 0.f;
	Character->GetMesh()->GetAnimInstance()->Montage_JumpToSection(MontageSectionEndLoop, RunningInteraction.Montage);
	GetWorld()->GetTimerManager().SetTimer(InteractionsTimers[CurrentInteractionIndex],
		EndTimerDelegates[CurrentInteractionIndex], StandUpDuration, false);
	
	return FNpcInteractionStopResult(StandUpDuration);
}

void ANpcGenericInteractionActor::OnInteractionFinished(int Position)
{
	auto Character = InteractingCharacters[Position].Character;
	if (IsValid(Character))
	{
		Character->GetCapsuleComponent()->SetCollisionProfileName(ProfileCharacterCapsule);
		Character->GetCharacterMovement()->SetAvoidanceEnabled(true);
		FinishInteraction(Character);
		if (InteractionStateChangedEvent.IsBound())
			InteractionStateChangedEvent.Broadcast(this, Character, ENpcActivityLatentActionState::Completed);
	}
	else InteractingCharacters[Position].Reset();
}
