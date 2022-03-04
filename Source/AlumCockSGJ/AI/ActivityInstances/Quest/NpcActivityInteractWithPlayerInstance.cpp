#include "NpcActivityInteractWithPlayerInstance.h"

void UNpcActivityInteractWithPlayerInstance::Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor,
	AAIController* AIController)
{
	Super::Setup(NpcBehaviorDescriptor, AIController);
	QuestSettings = Cast<UNpcQuestActivityInteractWithPlayerSettings>(NpcBehaviorDescriptor.NpcActivitySettings);
}

FGameplayTag UNpcActivityInteractWithPlayerInstance::GetStateTag() const
{
	return QuestSettings->InitialNpcState;
}
