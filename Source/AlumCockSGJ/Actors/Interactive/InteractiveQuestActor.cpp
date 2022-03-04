#include "InteractiveQuestActor.h"

#include "Characters/BaseCharacter.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Components/QuestGiverComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Subsystems/QuestSubsystem.h"

AInteractiveQuestActor::AInteractiveQuestActor()
{
	QuestGiverComponent = CreateDefaultSubobject<UQuestGiverComponent>(TEXT("QuestGiver"));
	AddOwnedComponent(QuestGiverComponent);
}

bool AInteractiveQuestActor::InteractWithPlayer(APlayerCharacter* Interactor)
{
	auto PlayerController = Cast<ABasePlayerController>(Interactor->Controller);
	if (!IsValid(PlayerController))
		return false;
	
	if (IsValid(InteractSFX))
		UGameplayStatics::PlaySound2D(GetWorld(), InteractSFX);
	
	auto QuestSystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
	QuestSystem->OnActorInteracted(this);
	QuestGiverComponent->GiveQuests();
	return true;
}

void AInteractiveQuestActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
}

bool AInteractiveQuestActor::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GameplayTags.HasTag(TagToCheck);
}

bool AInteractiveQuestActor::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAllExact(TagContainer);
}

bool AInteractiveQuestActor::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAnyExact(TagContainer);
}
