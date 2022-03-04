#include "DialogueSubsystem.h"

#include "FMODBlueprintStatics.h"
#include "NpcSubsystem.h"
#include "QuestSubsystem.h"
#include "WorldStateSubsystem.h"
#include "AI/Data/NpcDTR.h"
#include "AI/Data/NpcPlayerInteractionDTR.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Interfaces/DialogueParticipantManager.h"
#include "UI/DialogueWidget.h"

void UDialogueSubsystem::RegisterPlayerController(ABasePlayerController* NewController)
{
	PlayerController = NewController;
}

bool UDialogueSubsystem::StartDialogue(const FNpcPlayerInteractionDTR* InteractionData,
	ABaseCharacter* InitialNpc, FNpcDTR* InitialNpcDTR)
{
	if(InteractionData->Dialogue.DialogueScenarios.Num() <= 0)
		return false;

	bool bCanStart = TryStartDialogue(InteractionData->Dialogue);
	if (!bCanStart)
		return false;
	
	Participants.Reset();
	Participants.SetNum(2 + InteractionData->Dialogue.ExtraParticipants.Num());
	auto PlayerCharacter = PlayerController->GetControlledCharacter();
	Participants[0] = FDialogueParticipant(PlayerCharacter, PlayerCharacter->GetNpcDTRH().GetRow<FNpcDTR>(""));
	Participants[1] = FDialogueParticipant(InitialNpc, InitialNpcDTR);
	
	auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
	for (auto i = 0; i < InteractionData->Dialogue.ExtraParticipants.Num(); i++)
	{
		auto ParticipantCharacter = NpcSubsystem->GetNpc(InteractionData->Dialogue.ExtraParticipants[i]);
		Participants[2 + i] = FDialogueParticipant(ParticipantCharacter, InteractionData->Dialogue.ExtraParticipants[i].GetRow<FNpcDTR>(""));
	}
	
	bLocking = InteractionData->bLocking;
	FVector NpcLocation = FVector::ZeroVector;
	for (auto Participant : Participants)
	{
		if (!Participant.Character)
			continue;
		
		auto DialogueParticipant = Cast<IDialogueParticipantManager>(Participant.Character->GetController());
		if (DialogueParticipant)
			DialogueParticipant->OnDialogueStarted(InteractionData->bFollowPlayer);

		if (NpcLocation == FVector::ZeroVector && Participant.Character->GetController() != PlayerController)
			NpcLocation = Participant.Character->GetActorLocation();
	}

	PlayerController->OnDialogueStarted(bLocking, NpcLocation);
	StartNextLine();
	return true;
}

bool UDialogueSubsystem::StartAutonomousDialogue(const FQuestDialogue& Dialogue)
{
	bool bCanStart = TryStartDialogue(Dialogue);
	if (!bCanStart)
		return false;

	Participants.Reset();
	Participants.SetNum(1 + Dialogue.ExtraParticipants.Num());
	auto PlayerCharacter = PlayerController->GetControlledCharacter();
	Participants[0] = FDialogueParticipant(PlayerCharacter, PlayerCharacter->GetNpcDTRH().GetRow<FNpcDTR>(""));
	auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
	for (auto i = 0; i < Dialogue.ExtraParticipants.Num(); i++)
	{
		auto ParticipantCharacter = NpcSubsystem->GetNpc(Dialogue.ExtraParticipants[i]);
		Participants[1 + i] = FDialogueParticipant(ParticipantCharacter, Dialogue.ExtraParticipants[i].GetRow<FNpcDTR>(""));
	}

	bLocking = false;
	PlayerController->OnDialogueStarted(bLocking, Participants[0].Character->GetActorLocation());
	StartNextLine();
	return true;
}

bool UDialogueSubsystem::TryStartDialogue(const FQuestDialogue& NewDialogue)
{
	if (NewDialogue.DialogueScenarios.Num() <= 0)
		return false;

	if (bDialogueRunning)
		FinishDialogue();
	
	// ResetDialogue();
	int DialogueScenarioIndex = FMath::RandRange(0, NewDialogue.DialogueScenarios.Num() - 1);
	CurrentDialogue = NewDialogue.DialogueScenarios[DialogueScenarioIndex].DialogueLines;
	
	TotalDialogueLines = CurrentDialogue.Num();
	CurrentLineIndex = 0;
	bDialogueRunning = true;
	return true;
}

void UDialogueSubsystem::InterruptDialogue(const ABaseCharacter* InterruptingParticipant)
{
	if (!bDialogueRunning)
		return;
	
	for (const auto& ParticipantData : Participants)
	{
		if (ParticipantData.Character == InterruptingParticipant)
		{
			FinishDialogue();
			return;
		}
	}
}

void UDialogueSubsystem::FinishDialogue()
{
	CurrentLineIndex = -1;
	TotalDialogueLines = 0;
	bDialogueRunning = false;
	GetWorld()->GetTimerManager().ClearTimer(DialogueLineTimer);
	PlayerController->OnDialogueEnded(Participants, bLocking);
	for (const auto& ParticipantData : Participants)
	{
		auto ParticipantAnimInstance = ParticipantData.Character->GetMesh()->GetAnimInstance();
		if (ParticipantData.RunningMontage && ParticipantAnimInstance->Montage_IsPlaying(ParticipantData.RunningMontage))
			ParticipantAnimInstance->Montage_Stop(0.2f, ParticipantData.RunningMontage);
		
		auto DialogueParticipant = Cast<IDialogueParticipantManager>(ParticipantData.Character->GetController());
		if (DialogueParticipant)
			DialogueParticipant->OnDialogueEnded();

		ParticipantData.Character->InteruptVoiceLine();
	}
}

void UDialogueSubsystem::StartNextLine()
{
	if (CurrentLineIndex == TotalDialogueLines)
	{
		FinishDialogue();
		Participants.Empty();
		return;
	}

	const auto& DialogueLine = CurrentDialogue[CurrentLineIndex];
	int SafeParticipantIndex = DialogueLine.ParticipantIndex % Participants.Num();
	auto Participant = Participants[SafeParticipantIndex].NpcDTR;
	PlayerController->GetPlayerHUDWidget()->GetDialogueWidget()->ShowDialogueLine(DialogueLine, Participant);
	float ExpectedDuration = DialogueLine.LineDuration;

	if (DialogueLine.GestureOptions.Num() > 0)
	{
		int GestureIndex = FMath::RandRange(0, DialogueLine.GestureOptions.Num() - 1);
		if (Participants[DialogueLine.ParticipantIndex].Character)
		{
			Participants[DialogueLine.ParticipantIndex].RunningMontage = DialogueLine.GestureOptions[GestureIndex];
			ExpectedDuration = Participants[DialogueLine.ParticipantIndex].Character->PlayAnimMontage(DialogueLine.GestureOptions[GestureIndex]);
		}
	}
	
	if (DialogueLine.VoiceLine)
	{
		if (Participants[SafeParticipantIndex].Character)
		{
			ExpectedDuration = Participants[SafeParticipantIndex].Character->PlayFmodEvent(DialogueLine.VoiceLine);
			// PlayingFmodEvent = UFMODBlueprintStatics::PlayEventAttached(DialogueLine.VoiceLine, Participants[SafeParticipantIndex].Character->GetMesh(), NAME_None,
			// 	FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, true, true);
			// ExpectedDuration = PlayingFmodEvent->GetLength() / 1000.f;
		}
		else
		{
			UFMODBlueprintStatics::PlayEvent2D(GetWorld(), DialogueLine.VoiceLine, true);
			// TODO set duration as well
		}
	}

	auto QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
	if (DialogueLine.TriggerQuests.Num() > 0)
	{
		for (const auto& QuestDTRH : DialogueLine.TriggerQuests)
		{
			if (!QuestDTRH.IsNull())
				QuestSubsystem->AddQuest(QuestDTRH);
		}
	}

	if (DialogueLine.TriggerQuestsEvents.Num() > 0)
	{
		for (const auto& QuestEventDTRH : DialogueLine.TriggerQuestsEvents)
		{
			if (!QuestEventDTRH.IsNull())
				QuestSubsystem->QuestEventOccured(QuestEventDTRH);
		}	
	}

	for (const auto& ParticipantStateEffect : DialogueLine.ParticipantStateEffects)
	{
		if (!ParticipantStateEffect.StateEffect.IsEmpty())
		{
			Participants[ParticipantStateEffect.ParticipantIndex].Character->ChangeGameplayTags(ParticipantStateEffect.StateEffect,
				ParticipantStateEffect.bAppendState);
		}
	}

	auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	// TODO I really really doubt DS should change the WS
	for (const auto& WorldStateEffect : DialogueLine.WorldStateEffects)
	{
		if (!WorldStateEffect.StateEffect.IsEmpty())
			WSS->ChangeWorldState(WorldStateEffect.StateEffect, WorldStateEffect.bAppendState);
	}

	if (DialogueLine.RunBehavior.BehaviorDescriptor.BehaviorDescriptor.BehaviorTree)
	{
		auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
		int ParticipantBehaviorIndex = DialogueLine.RunBehavior.ParticipantIndex % Participants.Num();
		NpcSubsystem->RunQuestBehavior(Participants[ParticipantBehaviorIndex].Character, DialogueLine.RunBehavior.BehaviorDescriptor);
	}
	
	CurrentLineIndex++;
	GetWorld()->GetTimerManager().SetTimer(DialogueLineTimer, this, &UDialogueSubsystem::StartNextLine, ExpectedDuration);
}

void UDialogueSubsystem::SkipLine()
{
	int PreviousLine = CurrentLineIndex - 1;
	if (PreviousLine >= 0 && PreviousLine < CurrentDialogue.Num())
	{
		const auto& DialogueLine = CurrentDialogue[PreviousLine];
		if (DialogueLine.VoiceLine && Participants[DialogueLine.ParticipantIndex].Character)
			Participants[DialogueLine.ParticipantIndex].Character->InteruptVoiceLine();
	}
	
	StartNextLine();
}
