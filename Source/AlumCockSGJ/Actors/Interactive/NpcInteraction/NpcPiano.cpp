#include "NpcPiano.h"

#include "CommonConstants.h"
#include "Characters/BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANpcPiano::ANpcPiano()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PianoMusicComponent"));
	AudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANpcPiano::BeginPlay()
{
	Super::BeginPlay();
	AudioComponent->Stop();
}

bool ANpcPiano::TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag)
{
	auto ActualInteractionTag = DefaultInteractionTag;
	bool bCanStartInteracting = Super::TryStartNpcInteraction_Implementation(Character, ActualInteractionTag);
	if (!bCanStartInteracting)
		return false;

	const FCharacterInteractionParameters& InteractionParameters = Character->GetInteractionParameters(ActualInteractionTag);
	if (!IsValid(InteractionParameters.InteractionMontage))
		return false;

	auto PlayMontage = InteractionParameters.InteractionMontage;
	Character->GetCapsuleComponent()->SetCollisionProfileName(ProfileNoCollisionInteraction);
	
	FVector RelativeCharacterLocation = ActorInteractionOptions[0].InteractionPosition;
	FRotator Rotator = GetActorRotation();
	Character->UnsetDesiredRotation();
	Character->SetActorRotation(Rotator); // won't need with a proper sit down animation with root motion
	Character->SetActorLocation(GetActorLocation() + Rotator.RotateVector(RelativeCharacterLocation));
	
	Character->PlayAnimMontage(PlayMontage);
	BeginInteraction(0, FRunningInteractionData(Character, PlayMontage));
	float SitDuration = PlayMontage->GetSectionLength(PlayMontage->GetSectionIndex("Default"));
	GetWorld()->GetTimerManager().SetTimer(SitTimer, this, &ANpcPiano::OnSat, SitDuration);
	// Character->GetCharacterMovement()->bUseRVOAvoidance = 0;
	return true;
}

FNpcInteractionStopResult ANpcPiano::StopNpcInteraction_Implementation(ABaseCharacter* Character, bool bInterupted)
{
	if (!IsValid(Character))
		return FNpcInteractionStopResult();

	AudioComponent->Stop();
	auto& InteractionData = InteractingCharacters[0];
	auto PlayMontage = InteractionData.Montage;
	if (!PlayMontage)
		return FNpcInteractionStopResult(0.f);
	
	Character->GetMesh()->GetAnimInstance()->Montage_JumpToSection(MontageSectionEndLoop, PlayMontage);
	float StandUpDuration = PlayMontage->GetSectionLength(PlayMontage->GetSectionIndex(MontageSectionEndLoop));
	GetWorld()->GetTimerManager().SetTimer(StandUpTimer,this, &ANpcPiano::OnStoodUp, StandUpDuration);
	
	return FNpcInteractionStopResult(StandUpDuration);
}

void ANpcPiano::OnSat()
{
	AudioComponent->Play();
}

void ANpcPiano::OnStoodUp()
{
	auto Character = InteractingCharacters[0].Character;
	Character->GetCapsuleComponent()->SetCollisionProfileName(ProfileCharacterCapsule);
	// Character->GetCharacterMovement()->bUseRVOAvoidance = 1;
	FinishInteraction(Character);
	if (InteractionStateChangedEvent.IsBound())
		InteractionStateChangedEvent.Broadcast(this, Character, ENpcActivityLatentActionState::Completed);
}
