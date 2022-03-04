#include "NpcSubsystem.h"

#include "FMODBlueprintStatics.h"
#include "AI/Data/NpcDTR.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/NpcController.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Interfaces/DatatableInitializablePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

void UNpcSubsystem::RegisterNpc(const FDataTableRowHandle& DTRH, ABaseCharacter* Npc)
{
	if (DTRH.IsNull())
		return;

	auto NpcCharacter = Cast<INpcCharacter>(Npc);
	if (NpcCharacter && NpcCharacter->GetNpcLogicComponent())
		NpcCharacter->GetNpcLogicComponent()->NpcDiedEvent.AddUObject(this, &UNpcSubsystem::OnNpcDied);

	FNpcDTR* NpcDTR = DTRH.GetRow<FNpcDTR>("");
	if (!NpcDTR)
		return;

	checkf(!NpcDTR->bUnique || !Npcs.Contains(DTRH.RowName), TEXT("Attempting to spawn unique NPC %s again"), *DTRH.ToDebugString());
	
	Npcs.Add(DTRH.RowName, Npc);
}

void UNpcSubsystem::RegisterPlayerController(const ABasePlayerController* NewPlayerController)
{
	PlayerController = NewPlayerController;
}

FNpcReactResult UNpcSubsystem::TryReact(ABaseCharacter* Npc, const FGameplayTag& ReactionTag)
{
	FNpcReactResult Result;
	INpcCharacter* NpcCharacter = Cast<INpcCharacter>(Npc);
	if (!NpcCharacter)
		return Result;
	
	auto NpcDTR = NpcCharacter->GetNpcDTRH().GetRow<FNpcDTR>("");
	if (!NpcDTR || !NpcDTR->Reactions)
		return Result;
	
	auto ReactionOptions = NpcDTR->Reactions->NpcReactions.Find(ReactionTag);
	if (!ReactionOptions || ReactionOptions->NpcReactions.Num() <= 0)
		return Result;
	
	Result.ReactionIndex = FMath::RandRange(0, ReactionOptions->NpcReactions.Num() - 1);
	bool bCanStartReaction = NpcCharacter->StartReaction(ReactionTag, Result.ReactionIndex);
	if (!bCanStartReaction)
		return Result;

	const auto& RandomReaction = ReactionOptions->NpcReactions[Result.ReactionIndex];
	Result.ExpectedDuration = RandomReaction.Duration;
	if (RandomReaction.Gesture)
		Result.ExpectedDuration = Npc->PlayAnimMontage(RandomReaction.Gesture);

	if (RandomReaction.VoiceLine)
	{
		Result.ExpectedDuration = Npc->PlayFmodEvent(RandomReaction.VoiceLine);
		// auto PlayedEvent = UFMODBlueprintStatics::PlayEventAttached(RandomReaction.VoiceLine, Npc->GetCapsuleComponent(), NAME_None, FVector::ZeroVector,
		// 	EAttachLocation::KeepRelativeOffset, true, true, true);
	}

	UAISense_Hearing::ReportNoiseEvent(Npc->GetWorld(), Npc->GetActorLocation(),RandomReaction.SpeechLoudness,
		Npc, RandomReaction.SpeechRangeAI, ReactionTag.GetTagName());

	if (IsValid(PlayerController) && !RandomReaction.SpeechText.IsEmpty())
		PlayerController->ShowNpcSpeechSubtitles(NpcDTR->Name, RandomReaction.SpeechText, Npc->GetActorLocation(), RandomReaction.SpeechLoudness);

	Result.bStarted = true;
	return Result;
}

bool UNpcSubsystem::TryFinishReaction(ABaseCharacter* Npc, const FGameplayTag& ReactionType, int ReactionIndex, const FName& MontageEndLoopSectionName)
{
	if (ReactionIndex < 0)
		return false;
	
	INpcCharacter* NpcCharacter = Cast<INpcCharacter>(Npc);
	if (!NpcCharacter)
		return false;

	auto NpcDTR = NpcCharacter->GetNpcDTRH().GetRow<FNpcDTR>("");
	if (!NpcDTR || !NpcDTR->Reactions)
		return false;
	
	bool bReactionStopped = NpcCharacter->StopReaction(ReactionType);
	if (!bReactionStopped)
		return false;

	auto ReactionOptions = NpcDTR->Reactions->NpcReactions.Find(ReactionType);
	if (!ReactionOptions || ReactionOptions->NpcReactions.Num() <= 0)
		return true;
	
	auto& RunningReaction = ReactionOptions->NpcReactions[ReactionIndex];
	if (!RunningReaction.Gesture)
		return true;
	
	auto AnimInstance = Npc->GetMesh()->GetAnimInstance();
	if (AnimInstance->Montage_IsPlaying(RunningReaction.Gesture))
	{
		if (RunningReaction.Gesture->IsValidSectionName(MontageEndLoopSectionName))
			AnimInstance->Montage_JumpToSection(MontageEndLoopSectionName, RunningReaction.Gesture);
		else AnimInstance->Montage_Stop(0.2f, RunningReaction.Gesture);
	}
	
	return true;
}

bool UNpcSubsystem::TryRunQuestBehavior(const FQuestActionNpcRunBehavior& RunBehaviorData)
{
	TArray<ABaseCharacter*> RelevantNPCs; 
	Npcs.MultiFind(RunBehaviorData.NpcDTRH.RowName, RelevantNPCs);
	if (RelevantNPCs.Num() <= 0)
		return false;

	if (RunBehaviorData.bFirstOnly)
	{
		if (RunBehaviorData.WithTagOnly.IsValid())
		{
			for (const auto NPC : RelevantNPCs)
			{
				IGameplayTagAssetInterface* GameplayTagCharacter = Cast<IGameplayTagAssetInterface>(NPC);
				if (!GameplayTagCharacter)
					continue;

				if (GameplayTagCharacter->HasMatchingGameplayTag(RunBehaviorData.WithTagOnly))
				{
					RunQuestBehavior(NPC, RunBehaviorData.NpcQuestBehaviorDescriptor);
					return true;
				}
			}	
		}
		else
		{
			RunQuestBehavior(RelevantNPCs[0], RunBehaviorData.NpcQuestBehaviorDescriptor);
			return true;
		}
	}
	else
	{
		for (const auto NPC : RelevantNPCs)
		{
			if (RunBehaviorData.WithTagOnly.IsValid())
			{
				IGameplayTagAssetInterface* GameplayTagCharacter = Cast<IGameplayTagAssetInterface>(NPC);
				if (!GameplayTagCharacter)
					continue;

				if (!GameplayTagCharacter->HasMatchingGameplayTag(RunBehaviorData.WithTagOnly))
					continue;
			}
			
			RunQuestBehavior(NPC, RunBehaviorData.NpcQuestBehaviorDescriptor);
		}
	}
	
	return true;
}

ABaseCharacter* UNpcSubsystem::FindNpc(const FDataTableRowHandle& NpcDTRH, bool bSpawnNew)
{
	FNpcDTR* NpcDTR = NpcDTRH.GetRow<FNpcDTR>("");
	if (!NpcDTR)
		return nullptr;
	
	ABaseCharacter* Npc = nullptr;
	if (NpcDTR->bUnique)
	{
		auto SuitableNpcs = Npcs.Find(NpcDTRH.RowName);
		if (SuitableNpcs)
			Npc = SuitableNpcs[0];	
	}
	else if (bSpawnNew)
	{
		Npc = nullptr;
	}
	else 
	{
		TArray<ABaseCharacter*> SuitableNpcs;
		Npcs.MultiFind(NpcDTRH.RowName, SuitableNpcs);
		if (SuitableNpcs.Num() > 0)
		{
			FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();
			float MaxDistance = FLT_MAX;
			for (const auto SuitableNpc : SuitableNpcs)
			{
				float TestSqDistance = FVector::DistSquared(PlayerLocation, SuitableNpc->GetActorLocation()) ;
				if (TestSqDistance < MaxDistance)
				{
					MaxDistance = TestSqDistance;
					Npc = SuitableNpc;
				}
			}
		}
	}

	return Npc;
}

ABaseCharacter* UNpcSubsystem::SpawnNpc(const FQuestActionSpawnNpcs& SpawnNpcAction, const FVector& SpawnLocation)
{
	auto& NpcDTRH = SpawnNpcAction.NpcDTRH;
	if (NpcDTRH.IsNull())
		return nullptr;

	ABaseCharacter* Npc = FindNpc(NpcDTRH, SpawnNpcAction.bSpawnNew);
	if (Npc)
	{
		auto NpcController = Cast<INpcController>(Npc->GetController());
		if (NpcController)
			NpcController->SetAIEnabled(false);
		
		Npc->SetActorLocation(SpawnLocation);
		if (NpcController)
			NpcController->SetAIEnabled(true);
	}
	else
	{
		FActorSpawnParameters ActorSpawnParameters;
		FNpcDTR* NpcDTR = NpcDTRH.GetRow<FNpcDTR>("");
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Npc = GetWorld()->SpawnActor<ABaseCharacter>(NpcDTR->SpawnClass, SpawnLocation, FRotator::ZeroRotator, ActorSpawnParameters);
		
		auto DatatableInitializablePawn = Cast<IDatatableInitializablePawn>(Npc);
		if (DatatableInitializablePawn)
			DatatableInitializablePawn->SetNpcDTRH(SpawnNpcAction.NpcDTRH);
		
		Npc->SpawnDefaultController();
	}

	if (!Npc)
		return nullptr;
	
	if (!SpawnNpcAction.WithTags.IsEmpty())
	{
		auto GameplayTagActor = Cast<IGameplayTagActor>(Npc);
		if (GameplayTagActor)
			GameplayTagActor->ChangeGameplayTags(SpawnNpcAction.WithTags);
	}

	return Npc;
}

FRotator UNpcSubsystem::GetFacePlayerViewpointRotation(const FVector& ObserverLocation) const
{
	if (PlayerController)
	{
		auto PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
			return (PlayerPawn->GetPawnViewLocation() - ObserverLocation).Rotation();
	}
	
	return FRotator::ZeroRotator;
}

bool UNpcSubsystem::RunQuestBehavior(ABaseCharacter* Npc, const FNpcQuestBehaviorDescriptor& BehaviorDescriptor) const
{
	auto ActivityManager = Cast<INpcActivityManager>(Npc->GetController());
	if (!ActivityManager)
		return false;

	ActivityManager->RunQuestActivity(BehaviorDescriptor);
	return true;
}

ABaseCharacter* UNpcSubsystem::GetNpc(const FDataTableRowHandle& NpcDTRH)
{
	return FindNpc(NpcDTRH, false);
}

void UNpcSubsystem::OnNpcDied(const FDataTableRowHandle& NpcDTRH, ABaseCharacter* NpcCharacter)
{
	Npcs.RemoveSingle(NpcDTRH.RowName, NpcCharacter);
}
