#include "NpcConversationComponent.h"

#include "FMODBlueprintStatics.h"
#include "AI/Data/NpcDTR.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Subsystems/WorldStateSubsystem.h"

void UNpcConversationComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<ABaseCharacter>(GetOwner());
	Character->CharacterDeathEvent.AddUObject(this, &UNpcConversationComponent::OnCharacterDied);
	OwnerNpcCharacter = Cast<INpcCharacter>(GetOwner());
}

bool UNpcConversationComponent::RequestConversation(UNpcConversationComponent* OtherConversationComponentIn)
{
	FNpcDTR* NpcDTR = OwnerNpcCharacter->GetNpcDTRH().GetRow<FNpcDTR>("");
	if (!NpcDTR->ConversationSettings)
		return false;

	auto ConversationSettings = NpcDTR->ConversationSettings;
	FGameplayTagContainer NpcState;
	Character->GetOwnedGameplayTags(NpcState);
	if (bInDialogue || !ConversationSettings->ConversationTagFilter.Matches(NpcState))
		return false;

	OtherConversationComponent = OtherConversationComponentIn;
	const auto& OtherNpcDTRH = OtherConversationComponent->GetNpcDTRH();
	auto WSS = GetWorld()->GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	const auto& WorldState = WSS->GetWorldState();
	int BestWorldStateSimilarityScore = 1;
	TArray<FConversationOption> ConversationOptions;
	
	for (const auto& ConversationScripts : ConversationSettings->Conversations)
	{
		if (ConversationScripts.WithCharacter == OtherNpcDTRH)
		{
			for (const auto& Conversation : ConversationScripts.ConversationsScripts)
			{
				FConversationOption NewOption;
				NewOption.Conversation = &Conversation;
				NewOption.WorldStateSimilarityScore = Conversation.AtWorldState.FilterExact(WorldState).Num();
				if (NewOption.WorldStateSimilarityScore >= BestWorldStateSimilarityScore)
				{
					BestWorldStateSimilarityScore = NewOption.WorldStateSimilarityScore;
					ConversationOptions.Add(NewOption);
				}
			}
			
			break;
		}
	}

	if (ConversationOptions.Num() <= 0)
		return false;
	
	ConversationOptions.Sort();
	int BestConverastionIndex = FMath::RandRange(0, FMath::Min(ConversationOptions.Num() - 1, 3));
	CurrentConversation = ConversationOptions[BestConverastionIndex];

	bDialogueMaster = true;
	OtherConversationComponent->bDialogueMaster = false;
	
	StartConversation();
	return true;
}

void UNpcConversationComponent::StartConversation()
{
	CurrentIndex = 0;
	SetConversationState(true);
	/* TODO
	 *  Tell BT to get into dialogue mode
	 *  Master picks dialogue position by EQS and tells the other
	 *  Both get there
	 *  Continue talking
	 */
	OtherConversationComponent->SetConversationState(true);
	AdvanceConversation();
}

void UNpcConversationComponent::HandleDialogueLine(const FNpcDialogueLine& DialogueLine)
{
	if (DialogueLine.GestureOptions.Num() > 0)
		Character->PlayAnimMontage(DialogueLine.GestureOptions[FMath::RandRange(0, DialogueLine.GestureOptions.Num() - 1)]);

	if (DialogueLine.VoiceLine)
	{
		UFMODBlueprintStatics::PlayEventAttached(DialogueLine.VoiceLine, Character->GetMesh(), NAME_None,
			FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, true, true);
	}
	else if (DialogueLine.Speech)
	{
		UGameplayStatics::PlaySoundAtLocation(Character, DialogueLine.Speech, Character->GetActorLocation());
	}

	// TODO 3 line traces from speaker to character. If any hits - show subtitles
	if (false)
	{
		
	}
}

void UNpcConversationComponent::AdvanceConversation()
{
	if (!bDialogueMaster)
		return;
	
	if (CurrentIndex == CurrentConversation.Conversation->DialogueLines.Num())
	{
		FinishConversation();
		OtherConversationComponent->FinishConversation();
	}
	
	auto& NpcDialogueLine = CurrentConversation.Conversation->DialogueLines[CurrentIndex];
	if (NpcDialogueLine.ParticipantIndex == 1)
		OtherConversationComponent->HandleDialogueLine(NpcDialogueLine);
	else HandleDialogueLine(NpcDialogueLine);

	CurrentIndex++;
	GetWorld()->GetTimerManager().SetTimer(LineTimer, this, &UNpcConversationComponent::AdvanceConversation, NpcDialogueLine.LineDuration);
}


void UNpcConversationComponent::FinishConversation()
{
	// TODO tell AI to drop out of conversation and get back to its business
	SetConversationState(false);
	if (bDialogueMaster)
		OtherConversationComponent->SetConversationState(false);
}

void UNpcConversationComponent::OnCharacterDied(const ABaseCharacter* BaseCharacter)
{
	if (IsValid(OtherConversationComponent))
		OtherConversationComponent->FinishConversation();
}

void UNpcConversationComponent::SetConversationState(bool bActive)
{
	bInDialogue = bActive;
	auto GameplayTagActor = Cast<IGameplayTagActor>(GetOwner());
	GameplayTagActor->ChangeGameplayTags(InDialogueTag.GetSingleTagContainer(), bActive);
}
