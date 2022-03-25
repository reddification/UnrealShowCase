#include "AICitizenReactionsComponent.h"

#include "AIController.h"
#include "CommonConstants.h"
#include "AI/Controllers/AIBaseController.h"
#include "AI/Data/NpcDTR.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/FeelingsPropagator.h"
#include "Characters/BaseCharacter.h"
#include "Characters/PlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

UAICitizenReactionsComponent::UAICitizenReactionsComponent()
{
	PrimaryComponentTick.bCanEverTick = 1;
}

void UAICitizenReactionsComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeNpc();
}

void UAICitizenReactionsComponent::InitializeNpc()
{
	auto NpcCharacter = Cast<INpcCharacter>(ControlledCharacter);
	if (!NpcCharacter /*|| !NpcCharacter->IsAiEnabled()*/)
		return;
	
	auto NpcDTR = NpcCharacter->GetNpcDTRH().GetRow<FNpcDTR>("");
	if (!NpcDTR)
		return;
	
	AnxietySettings = NpcDTR->FeelingSettingsBundle->AnxietySettings;
	InterestSettings = NpcDTR->FeelingSettingsBundle->InterestSettings;
	FeelingPropagationSettings = NpcDTR->FeelingSettingsBundle->FeelingPropagationSettings;
	ReactionSourceEndangermentThresholds = NpcDTR->FeelingSettingsBundle->ReactionSourceEndangermentThresholds;
	PlayerInteractionSettings = NpcDTR->FeelingSettingsBundle->NpcInteractWithPlayerAnxietyEffect;
	
	// AnxietyReactions.SetNum(AnxietySettings->FeelingMemorySize);
	// InterestReactions.SetNum(InterestSettings->FeelingMemorySize);
	PropagationHistory.SetNum(FeelingPropagationSettings->PropagationMemorySize);
	AnxietyReactions.Add(EReactionSource::Sight, TArray<FReactionEvent>());
	AnxietyReactions.Add(EReactionSource::Sound, TArray<FReactionEvent>());
	AnxietyReactions.Add(EReactionSource::Damage, TArray<FReactionEvent>());
	InterestReactions.Add(EReactionSource::Sight, TArray<FReactionEvent>());
	InterestReactions.Add(EReactionSource::Sound, TArray<FReactionEvent>());
	InterestReactions.Add(EReactionSource::Damage, TArray<FReactionEvent>());
	
	AnxietyReactions[EReactionSource::Sight].SetNum(NpcDTR->FeelingSettingsBundle->FeelingMemorySize);
	AnxietyReactions[EReactionSource::Sound].SetNum(NpcDTR->FeelingSettingsBundle->FeelingMemorySize);
	AnxietyReactions[EReactionSource::Damage].SetNum(NpcDTR->FeelingSettingsBundle->FeelingMemorySize);
	InterestReactions[EReactionSource::Sight].SetNum(NpcDTR->FeelingSettingsBundle->FeelingMemorySize);
	InterestReactions[EReactionSource::Sound].SetNum(NpcDTR->FeelingSettingsBundle->FeelingMemorySize);
	InterestReactions[EReactionSource::Damage].SetNum(NpcDTR->FeelingSettingsBundle->FeelingMemorySize);
	
	LastAnxietyReactionsIndicies.Add(EReactionSource::Sight, 0);
	LastAnxietyReactionsIndicies.Add(EReactionSource::Sound, 0);
	LastAnxietyReactionsIndicies.Add(EReactionSource::Damage, 0);
	LastInterestReactionsIndicies.Add(EReactionSource::Sight, 0);
	LastInterestReactionsIndicies.Add(EReactionSource::Sound, 0);
	LastInterestReactionsIndicies.Add(EReactionSource::Damage, 0);
}

void UAICitizenReactionsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DecayFeeling(Anxiety, AnxietyHoldTime, DeltaTime, AnxietySettings->FeelingDecayRate,
		AnxietySettings->HoldDecayRate, BB_Anxiety);
	DecayFeeling(Interest, InterestHoldTime, DeltaTime, InterestSettings->FeelingDecayRate,
		InterestSettings->HoldDecayRate, BB_Interest);
	UpdateBlackboardFeelingState(Anxiety, AnxietySettings,AnxietyState, BB_AnxietyState);
	UpdateBlackboardFeelingState(Interest, InterestSettings, InterestState, BB_InterestState);
}

TArray<FVector> UAICitizenReactionsComponent::GetAfraidLocations() const
{
	// TArray<FVector> AfraidOfLocations = GetValidReactionsLocations(AnxietyReactions, LastAnxietyReactionIndex);
	auto AnxietySoundReactions = GetValidReactionsLocations(AnxietyReactions[EReactionSource::Sound],
		LastAnxietyReactionsIndicies[EReactionSource::Sound]);
	auto AnxietySightReactions = GetValidReactionsLocations(AnxietyReactions[EReactionSource::Sight],
		LastAnxietyReactionsIndicies[EReactionSource::Sight]);
	auto AnxietyDamageReactions = GetValidReactionsLocations(AnxietyReactions[EReactionSource::Damage],
		LastAnxietyReactionsIndicies[EReactionSource::Damage]);

	AnxietySoundReactions.Append(AnxietySightReactions.GetData(), AnxietySightReactions.Num());
	AnxietySoundReactions.Append(AnxietyDamageReactions.GetData(), AnxietyDamageReactions.Num());
	TArray<AActor*> ObservedActors;
	OwnerController->GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), ObservedActors);
	for (const auto ObservedActor : ObservedActors)
	{
		auto ObservedCharacter = Cast<ABaseCharacter>(ObservedActor);
		if (ObservedCharacter && ObservedCharacter->GetTeam() == ETeam::BadGuys)
			AnxietySoundReactions.Add(ObservedActor->GetActorLocation());
	}

	return AnxietySoundReactions;
}

TArray<FVector> UAICitizenReactionsComponent::GetInterestingLocations() const
{
	auto InterestingSoundReactions = GetValidReactionsLocations(InterestReactions[EReactionSource::Sound],
		LastInterestReactionsIndicies[EReactionSource::Sound]);
	auto InterestingSightReactions = GetValidReactionsLocations(InterestReactions[EReactionSource::Sight],
		LastInterestReactionsIndicies[EReactionSource::Sight]);
	auto InterestingDamageReactions = GetValidReactionsLocations(InterestReactions[EReactionSource::Damage],
		LastInterestReactionsIndicies[EReactionSource::Damage]);

	InterestingSoundReactions.Append(InterestingSightReactions.GetData(), InterestingSightReactions.Num());
	InterestingSoundReactions.Append(InterestingDamageReactions.GetData(), InterestingDamageReactions.Num());
	return InterestingSoundReactions;
}

FPropagatedFeelingsData UAICitizenReactionsComponent::GetAnxietyPropagation()
{
	return GetFeelingPropagation(Anxiety, AnxietyReactions, LastAnxietyReactionsIndicies, AnxietySettings);
}

FPropagatedFeelingsData UAICitizenReactionsComponent::GetInterestPropagation()
{
	return GetFeelingPropagation(Interest, InterestReactions, LastInterestReactionsIndicies, InterestSettings);
}

bool UAICitizenReactionsComponent::IsInDanger()
{
	float Now = GetWorld()->GetTimeSeconds();
	for (const auto& ReactionSourceEndangermentFeelCount : ReactionSourceEndangermentThresholds)
	{
		if (!AnxietyReactions.Contains(ReactionSourceEndangermentFeelCount.Key))
			continue;
		
		int RelevantDamageEventsCount = 0;
		for (const auto& ReactionEvent : AnxietyReactions[ReactionSourceEndangermentFeelCount.Key])
		{
			if (ReactionEvent.ExpiresAt > Now)
			{
				RelevantDamageEventsCount++;
				if (RelevantDamageEventsCount > ReactionSourceEndangermentFeelCount.Value)
					return true;
			}
		}
	}

	return false;
}

void UAICitizenReactionsComponent::CalmDownByPlayer(const APlayerCharacter* PlayerCharacter)
{
	float WorldTime = GetWorld()->GetTimeSeconds();
	if (IgnoreInteractPlayerUntil > WorldTime)
		return;

	float Distance = FVector::Distance(PlayerCharacter->GetActorLocation(), ControlledCharacter->GetActorLocation());
	float CalmDownEffect = PlayerInteractionSettings.WitnessPlayerCalmEffect * AnxietySettings->RangeToFeelingFactorDependency->GetFloatValue(Distance);
	CalmDown(CalmDownEffect);
	IgnoreInteractPlayerUntil = WorldTime + PlayerInteractionSettings.IgnoreInteractWithPlayerInterval;
}

void UAICitizenReactionsComponent::ReactToDamage(AActor* Actor, const FAIStimulus& Stimulus,
                                                 UBlackboardComponent* Blackboard)
{
	Super::ReactToDamage(Actor, Stimulus, Blackboard);
	if (Stimulus.IsExpired())
		return;

	bool bSeeDamagingActor = IsSeeingReactionInstigator(OwnerController, Actor);
	float CharacterHealth = ControlledCharacter->GetBaseCharacterAttributesComponent()->GetHealth();
	float DamageToHealthProportion = CharacterHealth > 0.f ? Stimulus.Strength / CharacterHealth : 1.f;
	
	FVector InstigatorLocation = Actor->GetActorLocation();
	auto Character = Cast<ABaseCharacter>(Actor);
	float PlayerFactor = Character && Character->GetTeam() == ETeam::Player ? 0.5f : 1.f;
	
	const float RawAnxietyChange = bSeeDamagingActor
		? DamageToHealthProportion * AnxietySettings->TakingDamageWhenSeeEnemyFeelChange
		: DamageToHealthProportion * AnxietySettings->TakingDamageFeelChange;
	
	const float RawInterestChange = bSeeDamagingActor
		? DamageToHealthProportion * InterestSettings->TakingDamageWhenSeeEnemyFeelChange
		: DamageToHealthProportion * InterestSettings->TakingDamageFeelChange;

	float AnxietyChange = ChangeFeeling(Anxiety, AnxietyHoldTime, RawAnxietyChange * PlayerFactor,
		AnxietySettings, AnxietyReactions, InstigatorLocation, BB_Anxiety, EReactionSource::Damage,
		AnxietyState, BB_AnxietyState);
	float InterestChange = ChangeFeeling(Interest, InterestHoldTime, RawInterestChange * PlayerFactor,
		InterestSettings, InterestReactions, InstigatorLocation, BB_Interest, EReactionSource::Damage,
		InterestState, BB_InterestState);
	RememberReactionEvent(AnxietyReactions, EReactionSource::Damage,  LastAnxietyReactionsIndicies,
		AnxietyChange, bSeeDamagingActor ? Actor : nullptr, InstigatorLocation, AnxietySettings->ReactionMemoryLifetimes);
	RememberReactionEvent(InterestReactions, EReactionSource::Damage, LastInterestReactionsIndicies, InterestChange,
		bSeeDamagingActor ? Actor : nullptr, InstigatorLocation, InterestSettings->ReactionMemoryLifetimes);
}

void UAICitizenReactionsComponent::OnSeePlayer(ABaseCharacter* PlayerCharacter)
{
	float WorldTime = GetWorld()->GetTimeSeconds();
	if (WorldTime > IgnoreWitnessPlayerUntil)
	{
		float Distance = FVector::Distance(PlayerCharacter->GetActorLocation(), ControlledCharacter->GetActorLocation());
		float CalmDownEffect = PlayerInteractionSettings.WitnessPlayerCalmEffect * AnxietySettings->RangeToFeelingFactorDependency->GetFloatValue(Distance);
		CalmDown(CalmDownEffect);
		IgnoreWitnessPlayerUntil = WorldTime + PlayerInteractionSettings.IgnoreWitnessPlayerInterval;
	}
}

void UAICitizenReactionsComponent::ReactToSight(AActor* Actor, const FAIStimulus& Stimulus,
                                                UBlackboardComponent* Blackboard)
{
	Super::ReactToSight(Actor, Stimulus, Blackboard);
	if (Stimulus.IsExpired())
		return;

	auto BaseCharacter = Cast<ABaseCharacter>(Actor);
	if (!BaseCharacter)
		return;
	
	switch (BaseCharacter->GetTeam())
	{
		case ETeam::GoodGuys:
			OnSeeGoodGuy(BaseCharacter);
		break;
		case ETeam::BadGuys:
			OnSeeBadGuy(BaseCharacter);
		break;
		case ETeam::Player:
			OnSeePlayer(BaseCharacter);
		break;
		default:
			break;
	}
}

void UAICitizenReactionsComponent::ReactToSound(AActor* Actor, const FAIStimulus& Stimulus)
{
	Super::ReactToSound(Actor, Stimulus);
	if (!Stimulus.WasSuccessfullySensed() || Actor == ControlledCharacter)
		return;

	bool bSeeingInstigatorActor = IsSeeingReactionInstigator(OwnerController, Actor);
	FVector SoundLocation = Stimulus.StimulusLocation;
	float AnxietyChange = 0.f;
	float InterestChange = 0.f;
	if(Stimulus.Tag == SoundStimulusTag_Shot)
	{
		InterestChange = ChangeFeeling(Interest, InterestHoldTime, InterestSettings->HearingShotFeelChange,
			InterestSettings, InterestReactions, SoundLocation, BB_Interest, EReactionSource::Sound,
			InterestState, BB_InterestState);
		AnxietyChange = ChangeFeeling(Anxiety, AnxietyHoldTime, AnxietySettings->HearingShotFeelChange,
			AnxietySettings, AnxietyReactions, SoundLocation, BB_Anxiety, EReactionSource::Sound,
			AnxietyState, BB_AnxietyState);	
	}
	else if (Stimulus.Tag == SoundStimulusTag_Explosion)
	{
		InterestChange = ChangeFeeling(Interest, InterestHoldTime, InterestSettings->HearingExplosionFeelChange,
			InterestSettings, InterestReactions, SoundLocation, BB_Interest, EReactionSource::Sound,
			InterestState, BB_InterestState);
		AnxietyChange = ChangeFeeling(Anxiety, AnxietyHoldTime, AnxietySettings->HearingExplosionFeelChange,
			AnxietySettings, AnxietyReactions, SoundLocation, BB_Anxiety, EReactionSource::Sound,
			AnxietyState, BB_AnxietyState);
	}
	else if (Stimulus.Tag == SoundStimulusTag_Step)
	{
		InterestChange = ChangeFeeling(Interest, InterestHoldTime, 0.005f,
			InterestSettings, InterestReactions, SoundLocation, BB_Interest, EReactionSource::Sound,
			InterestState, BB_InterestState);
	}
	else
	{
		auto SoundSourceCharacter = Cast<ABaseCharacter>(Actor);
		if (SoundSourceCharacter)
			PerceiveSoundTagReaction(Stimulus, SoundLocation, AnxietyChange, InterestChange, SoundSourceCharacter);
	}

	auto RememberedInstigatorActor = bSeeingInstigatorActor ? Actor : nullptr;
	// if (InterestChange > 0.f)
	RememberReactionEvent(InterestReactions, EReactionSource::Sound, LastInterestReactionsIndicies, InterestChange, RememberedInstigatorActor,
		SoundLocation, InterestSettings->ReactionMemoryLifetimes);

	// if (AnxietyChange > 0.f)
	RememberReactionEvent(AnxietyReactions, EReactionSource::Sound, LastAnxietyReactionsIndicies, AnxietyChange, RememberedInstigatorActor,
		SoundLocation, AnxietySettings->ReactionMemoryLifetimes);
}

void UAICitizenReactionsComponent::PerceiveSoundTagReaction(const FAIStimulus& Stimulus, FVector SoundLocation, float& AnxietyChange,
	float& InterestChange, ABaseCharacter* SoundSourceCharacter)
{
	FGameplayTag StimulusTag = FGameplayTag::RequestGameplayTag(Stimulus.Tag, false);
	if (!StimulusTag.IsValid())
		return;
	
	float WorldTime = GetWorld()->GetTimeSeconds();
	bool bValidTagReaction = true;
	FRememberedTagReaction* RememberedCharacterReaction = nullptr;
	for (auto& RememberedReaction : RememberedTagReactions)
	{
		if (RememberedReaction.Character == SoundSourceCharacter && RememberedReaction.ReactionTag == StimulusTag)
		{
			RememberedCharacterReaction = &RememberedReaction;
			if (RememberedReaction.RememberUntil > WorldTime)
			{
				bValidTagReaction = false;
				break;
			}
		}
	}

	if (!bValidTagReaction)
		return;
	
	if (RememberedCharacterReaction)
	{
		RememberedCharacterReaction->RememberUntil = WorldTime + 30.f;
	}
	else
	{
		FRememberedTagReaction NewRememberedTagReaction;
		NewRememberedTagReaction.Character = SoundSourceCharacter;
		NewRememberedTagReaction.ReactionTag = StimulusTag;
		NewRememberedTagReaction.RememberUntil = WorldTime + 30.f;
		RememberedTagReactions.Add(NewRememberedTagReaction);
	}
			
	auto ReactionInterestChange = InterestSettings->PerceivedReactionFeelingChanges.Find(StimulusTag);
	if (ReactionInterestChange)
	{
		InterestChange = ChangeFeeling(Interest, InterestHoldTime, *ReactionInterestChange,
		                               InterestSettings, InterestReactions, SoundLocation, BB_Interest, EReactionSource::Sound,
		                               InterestState, BB_InterestState);
	}

	auto ReactionAnxietyChange = AnxietySettings->PerceivedReactionFeelingChanges.Find(StimulusTag);
	if (ReactionAnxietyChange)
	{
		AnxietyChange = ChangeFeeling(AnxietyChange, AnxietyHoldTime, *ReactionAnxietyChange,
		                              AnxietySettings, AnxietyReactions, SoundLocation, BB_Anxiety, EReactionSource::Sound,
		                              AnxietyState, BB_AnxietyState);
	}
}

void UAICitizenReactionsComponent::OnSeeGoodGuy(ABaseCharacter* ObservedCharacter)
{
	if (!ObservedCharacter->IsAlive())
	{
		if (!WitnessedDeadCharacters.Contains(ObservedCharacter))
		{
			// TODO play stumble and backing-off montage like yo wtf he dead bruv
			WitnessedDeadCharacters.Emplace(ObservedCharacter);
			float AnxietyChange = ChangeFeeling(Anxiety, AnxietyHoldTime, AnxietySettings->WitnessDeadBodyFeelChange, AnxietySettings,
				AnxietyReactions, ObservedCharacter->GetActorLocation(), BB_Anxiety, EReactionSource::Sight, AnxietyState, BB_AnxietyState);
			float InterestChange = ChangeFeeling(Interest, InterestHoldTime, InterestSettings->WitnessDeadBodyFeelChange, InterestSettings, InterestReactions,
				ObservedCharacter->GetActorLocation(), BB_Interest, EReactionSource::Sight, InterestState, BB_InterestState);
			RememberReactionEvent(AnxietyReactions, EReactionSource::Sight, LastAnxietyReactionsIndicies, AnxietyChange, ObservedCharacter,
				ObservedCharacter->GetActorLocation(), AnxietySettings->ReactionMemoryLifetimes);
			RememberReactionEvent(InterestReactions, EReactionSource::Sight, LastInterestReactionsIndicies, InterestChange, ObservedCharacter,
				ObservedCharacter->GetActorLocation(), InterestSettings->ReactionMemoryLifetimes);
		}
	}
	else
	{
		auto FeelingsPropagator = Cast<IFeelingsPropagator>(ObservedCharacter->GetController());
		if (!FeelingsPropagator)
			return;
		
		// TODO bCanPropagateAnxiety and bCanPropagateInterest checks are stupid here. Refactor 
		float ActorsDistance = FVector::Distance(ControlledCharacter->GetActorLocation(), ObservedCharacter->GetActorLocation());

		bool bCanPropagateAnxiety = ActorsDistance <= FeelingPropagationSettings->MaxAnxietyPropagationDistance;
		bool bCanPropagateInterest = ActorsDistance <= FeelingPropagationSettings->MaxInterestPropagationDistance;

		if (!bCanPropagateAnxiety && !bCanPropagateInterest)
			return;

		if (!CanApplyPropagationFromActor(ObservedCharacter))
			return;

		const auto& AnxietyPropagation = FeelingsPropagator->GetAnxietyPropagation();
		const auto& InterestPropagation = FeelingsPropagator->GetInterestPropagation();

		FFeelingPropagationHistory NewPropagation;
		NewPropagation.Propagator = ObservedCharacter;
		NewPropagation.ExpiresAt = GetWorld()->GetTimeSeconds() + FeelingPropagationSettings->FeelingPropagationDelay;
		LastPropagationHistoryIndex = (LastPropagationHistoryIndex + 1) % PropagationHistory.Num();
		PropagationHistory[LastPropagationHistoryIndex] = NewPropagation;
		
		if (AnxietyPropagation.FeelingLevel > Anxiety)
		{
			ApplyPropagatedAnxiety(AnxietyPropagation, ActorsDistance);
		}
		else
		{
			// doesn't really look nice
			// float PropagationDistanceFactor = FeelingPropagationSettings->AnxietyPropagationFactorDistanceDependency
			// 	? FeelingPropagationSettings->AnxietyPropagationFactorDistanceDependency->GetFloatValue(ActorsDistance / FeelingPropagationSettings->MaxAnxietyPropagationDistance)
			// 	: 1.f;
			// float CalmDownEffect = AnxietyPropagation.FeelingLevel * FeelingPropagationSettings->FeelingPropagationFactor * PropagationDistanceFactor - Anxiety;
			// // CalmDownEffect must be < 0 always in this case
			// CalmDown(-CalmDownEffect);
		}
		
		if (InterestPropagation.FeelingLevel > Interest)
			ApplyPropagatedInterest(InterestPropagation, ActorsDistance);
	}
}

void UAICitizenReactionsComponent::OnSeeBadGuy(ABaseCharacter* BadGuy)
{
	auto EnemyAIController = Cast<AAIController>(BadGuy->GetController());
	float WorldTime = GetWorld()->GetTimeSeconds();
	for (const auto& WitnessedBadGuys : AnxietyReactions[EReactionSource::Sight])
	{
		if (WitnessedBadGuys.EventActor == BadGuy && WitnessedBadGuys.ExpiresAt > WorldTime)
			return;
	}
	
	TArray<AActor*> EnemyObservedActors;
	FActorPerceptionBlueprintInfo ActorPerceptionInfo;
	bool bEnemySeesMe = IsSeeingReactionInstigator(EnemyAIController, ControlledCharacter);
	FVector ActorLocation = BadGuy->GetActorLocation();
	float RawDelta = bEnemySeesMe ? AnxietySettings->SeeingEnemyReciprocallyFeelChange : AnxietySettings->SeeingEnemyFeelChange;
	float AnxietyChange = ChangeFeeling(Anxiety, AnxietyHoldTime, RawDelta, AnxietySettings, AnxietyReactions,
		ActorLocation, BB_Anxiety, EReactionSource::Sight, AnxietyState, BB_AnxietyState);
	RememberReactionEvent(AnxietyReactions, EReactionSource::Sight, LastAnxietyReactionsIndicies, AnxietyChange, BadGuy, ActorLocation,
		AnxietySettings->ReactionMemoryLifetimes);
}

void UAICitizenReactionsComponent::DecayFeeling(float& Feeling, float& FeelingHoldTime, float DeltaTime, float FeelDecayRate,
	float HoldDecayRate, const FName& BBKey)
{
	if (FeelingHoldTime > 0.f)
	{
		FeelingHoldTime -= DeltaTime * HoldDecayRate;
	}
	else if (Feeling > 0.f)
	{
		Feeling = FMath::Max(0.f, Feeling - FeelDecayRate * DeltaTime);
		OwnerController->GetBlackboardComponent()->SetValueAsFloat(BBKey, Feeling);
	}
}

float UAICitizenReactionsComponent::ChangeFeeling(float& Feeling, float& HoldTime, float RawDelta, const UFeelingSettings* FeelingSettings,
		const TMap<EReactionSource, TArray<FReactionEvent>>& ReactionEvents, const FVector& InstigatorLocation, const FName& FeelingLevelBBKey,
		EReactionSource ReactionSource, EFeelingState& FeelingState, const FName& StateBBKey)
{
	if (Feeling >= FeelingSettings->MaxFeelLevel)
		return 0.f;
	
	float Delta = RawDelta * FeelingSettings->MaxFeelLevel;
	if (ReactionSource != EReactionSource::Propagation && FeelingSettings->RangeToFeelingFactorDependency)
	{
		float Distance = FVector::Distance(InstigatorLocation, ControlledCharacter->GetActorLocation());
		Delta = Delta * FeelingSettings->RangeToFeelingFactorDependency->GetFloatValue(Distance);
	}

	if (ReactionSource != EReactionSource::Propagation)
	{
		const auto& FeelingReactionEvents = ReactionEvents[ReactionSource];
		int CountOfRelevantFeelingEvents = GetCountOfRelevantReactions(FeelingReactionEvents);
		if (CountOfRelevantFeelingEvents > 0 && FeelingSettings->ReactionsCountToFeelingFactorDependency.Contains(ReactionSource))
		{
			float CountRatio = (float)CountOfRelevantFeelingEvents / FeelingReactionEvents.Num();
			Delta = Delta * FeelingSettings->ReactionsCountToFeelingFactorDependency[ReactionSource]->GetFloatValue(CountRatio);
		}
	}

	if (Delta <= 0.f)
		return 0.f;
	
	// if (FeelingSettings->FeelingDeviation > 0.f)
	// {
	// 	float NormalizedDeviation = FeelingSettings->FeelingDeviation * FeelingSettings->MaxFeelLevel;
	// 	Delta = FMath::Max(Delta / 2.f, Delta + FMath::RandRange(-NormalizedDeviation, NormalizedDeviation));
	// }
	
	float NewFeeling = FMath::Clamp(Feeling + Delta, 0.f, FeelingSettings->MaxFeelLevel);
	float ActualDelta = NewFeeling - Feeling;
	Feeling = NewFeeling;
	if (FeelingSettings->FeelingToHoldTimeDependency)
		HoldTime = FMath::Min(FeelingSettings->FeelingToHoldTimeDependency->GetFloatValue(Feeling), FeelingSettings->MaxHoldTime);	
	else
		HoldTime = FMath::Min( HoldTime + Feeling * 10.f, FeelingSettings->MaxHoldTime); 
	
	UpdateBlackboardFeelingState(Feeling, FeelingSettings, FeelingState, StateBBKey);
	OwnerController->GetBlackboardComponent()->SetValueAsFloat(FeelingLevelBBKey, Feeling);
	return ActualDelta;
}

void UAICitizenReactionsComponent::ApplyPropagatedAnxiety(const FPropagatedFeelingsData& PropagationData, float Distance)
{
	if (PropagationData.FeelingLevel < Anxiety)
		return;
	
	float PropagationDistanceFactor = FeelingPropagationSettings->AnxietyPropagationFactorDistanceDependency
		? FeelingPropagationSettings->AnxietyPropagationFactorDistanceDependency->GetFloatValue(Distance / FeelingPropagationSettings->MaxAnxietyPropagationDistance)
		: 1.f;
	
	ApplyPropagatedFeeling(PropagationData, Anxiety, AnxietyReactions, AnxietyHoldTime, AnxietySettings,
		LastAnxietyReactionsIndicies, AnxietyState, PropagationDistanceFactor, BB_Anxiety, BB_AnxietyState);
}

void UAICitizenReactionsComponent::ApplyPropagatedInterest(const FPropagatedFeelingsData& PropagationData, float Distance)
{
	if (PropagationData.FeelingLevel < Interest)
		return;
	
	float PropagationDistanceFactor = FeelingPropagationSettings->InterestPropagationFactorDistanceDependency
		? FeelingPropagationSettings->InterestPropagationFactorDistanceDependency->GetFloatValue(Distance / FeelingPropagationSettings->MaxInterestPropagationDistance)
		: 1.f;
	
	ApplyPropagatedFeeling(PropagationData, Interest, InterestReactions, InterestHoldTime,
		InterestSettings, LastInterestReactionsIndicies, InterestState, PropagationDistanceFactor,
		BB_Interest,  BB_InterestState);
}

void UAICitizenReactionsComponent::ApplyPropagatedFeeling(const FPropagatedFeelingsData& PropagationData, float& Feeling,
	TMap<EReactionSource, TArray<FReactionEvent>>& FeelingReactions, float& FeelingHoldTime, const UFeelingSettings* FeelingSettings,
	TMap<EReactionSource, int>& LastIndicies, EFeelingState& FeelingState, float PropagationDistanceFactor,
	const FName& FeelingLevelBBKey, const FName& StateBBKey)
{
	float Change = PropagationData.FeelingLevel * FeelingPropagationSettings->FeelingPropagationFactor * PropagationDistanceFactor - Feeling;
	if (Change > 0.01f)
	{
		for (const auto& PropagatedReactionsSet : PropagationData.PropagatedReactions)
		{
			for (const auto& PropagatedReaction : PropagatedReactionsSet.Value)
			{
				LastIndicies[PropagatedReactionsSet.Key] = (LastIndicies[PropagatedReactionsSet.Key] + 1) % FeelingReactions[PropagatedReactionsSet.Key].Num();
				FeelingReactions[PropagatedReactionsSet.Key][LastIndicies[PropagatedReactionsSet.Key]] = PropagatedReaction;	
			}
		}

		ChangeFeeling(Feeling, FeelingHoldTime, Change, FeelingSettings, FeelingReactions,
			PropagationData.PropagatorLocation, FeelingLevelBBKey, EReactionSource::Propagation, FeelingState, StateBBKey);		
	}
}

void UAICitizenReactionsComponent::RememberReactionEvent(TMap<EReactionSource, TArray<FReactionEvent>>& EventMemory, EReactionSource ReactionSource,
		TMap<EReactionSource, int>& Indicies, float FeelingChange, const AActor* InstigatorActor,
		const FVector& InstigatorLocation, const TMap<EReactionSource, float>& ReactionMemoryLifetimes)
{
	float Now = GetWorld()->GetTimeSeconds();
	for (auto& ReactionEvent : EventMemory[ReactionSource])
	{
		if (ReactionEvent.ExpiresAt > Now && ReactionEvent.EventActor == InstigatorActor)
		{
			ReactionEvent.EventLocation = InstigatorLocation;
			ReactionEvent.ExpiresAt = Now + ReactionMemoryLifetimes[ReactionSource];
			ReactionEvent.FeelingChange = FeelingChange;
			return;
		}
	}
	int& Index = Indicies[ReactionSource];
	Index = (Index + 1) % EventMemory[ReactionSource].Num();
	FReactionEvent ReactionEvent;
	ReactionEvent.EventActor = InstigatorActor;
	ReactionEvent.FeelingChange = FeelingChange;
	ReactionEvent.EventLocation = InstigatorLocation;
	ReactionEvent.ExpiresAt = Now + ReactionMemoryLifetimes[ReactionSource];
	// ReactionEvent.ReactionSource = ReactionSource;
	EventMemory[ReactionSource][Index] = ReactionEvent;
}

bool UAICitizenReactionsComponent::IsSeeingReactionInstigator(AAIController* TestController, AActor* Actor) const
{
	ABaseCharacter* DamagingCharacter = Cast<ABaseCharacter>(Actor);
	if (!DamagingCharacter)
		return false;
	
	FActorPerceptionBlueprintInfo ActorPerceptionInfo;
	bool bSensingActor = TestController->GetPerceptionComponent()->GetActorsPerception(DamagingCharacter, ActorPerceptionInfo);
	if (bSensingActor)
		for (const auto& LastSensedStimuli : ActorPerceptionInfo.LastSensedStimuli)
			if (!LastSensedStimuli.IsExpired() && UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), LastSensedStimuli) == UAISense_Sight::StaticClass())
				return true;

	return false;
}

int UAICitizenReactionsComponent::GetCountOfRelevantReactions(const TArray<FReactionEvent>& ReactionsMemory) const
{
	int Result = 0;
	float WorldTime = GetWorld()->GetTimeSeconds();
	for (auto i = 0; i < ReactionsMemory.Num(); i++)
		if (ReactionsMemory[i].ExpiresAt > WorldTime)
			Result++;

	return Result;
}

TArray<FVector> UAICitizenReactionsComponent::GetValidReactionsLocations(const TArray<FReactionEvent>& Reactions, int LastIndex) const
{
	TArray<FVector> Result;
	float WorldTime = GetWorld()->GetTimeSeconds();
	long i = LastIndex;
	do
	{
		if (Reactions[i].ExpiresAt > WorldTime)
			Result.Emplace(Reactions[i].EventLocation);
		
		i--;
		if (i < 0)
			i = Reactions.Num() - 1;
	}
	while(i != LastIndex); 
	return Result;
}

FPropagatedFeelingsData UAICitizenReactionsComponent::GetFeelingPropagation(const float& Feeling,
    const TMap<EReactionSource, TArray<FReactionEvent>>& FeelingReactions, const TMap<EReactionSource, int>& LastIndicies,
    const UFeelingSettings* FeelingSettings) const
{
	if (!FeelingSettings)
		return FPropagatedFeelingsData();

	FPropagatedFeelingsData Result;
	Result.FeelingLevel = Feeling;
	Result.PropagatorLocation = ControlledCharacter->GetActorLocation();
	Result.PropagatedReactions.Add(EReactionSource::Sight, TArray<FReactionEvent>());
	Result.PropagatedReactions.Add(EReactionSource::Damage, TArray<FReactionEvent>());
	Result.PropagatedReactions.Add(EReactionSource::Sound, TArray<FReactionEvent>());
	
	Result.PropagatedReactions[EReactionSource::Sight].Reserve(FeelingPropagationSettings->PropagatedReactionsCount);
	Result.PropagatedReactions[EReactionSource::Damage].Reserve(FeelingPropagationSettings->PropagatedReactionsCount);
	Result.PropagatedReactions[EReactionSource::Sound].Reserve(FeelingPropagationSettings->PropagatedReactionsCount);
	
	FillFeelingPropagation(FeelingReactions, EReactionSource::Sight, LastIndicies, Result.PropagatedReactions);
	FillFeelingPropagation(FeelingReactions, EReactionSource::Sound, LastIndicies, Result.PropagatedReactions);
	FillFeelingPropagation(FeelingReactions, EReactionSource::Damage, LastIndicies, Result.PropagatedReactions);

	return Result;
}

void UAICitizenReactionsComponent::FillFeelingPropagation(const TMap<EReactionSource, TArray<FReactionEvent>>& FeelingReactions,
	EReactionSource ReactionSource, const TMap<EReactionSource, int>& LastIndicies, TMap<EReactionSource, TArray<FReactionEvent>>& PropagatedReactions) const
{
	int PropagatedCount = 0;
	float Now = GetWorld()->GetTimeSeconds();
	int LastFeelingReactionIndex = LastIndicies[ReactionSource];
	int MaxPropagationCount = FeelingPropagationSettings->PropagatedReactionsCount;
	int i = LastIndicies[ReactionSource];
	auto& Result = PropagatedReactions[ReactionSource];
	do
	{
		if (FeelingReactions[ReactionSource][i].ExpiresAt > Now)
		{
			Result.Emplace(FeelingReactions[ReactionSource][i]);
			PropagatedCount++;
		}
		
		i--;
		if (i < 0)
			i = FeelingReactions[ReactionSource].Num() - 1;
		
	} while (PropagatedCount < MaxPropagationCount && i != LastFeelingReactionIndex);
}


void UAICitizenReactionsComponent::UpdateBlackboardFeelingState(float Feeling, const UFeelingSettings* FeelingSettings, EFeelingState& FeelingState,
	const FName& BBKey)
{
	float NormalizedFeel = Feeling / FeelingSettings->MaxFeelLevel;
	EFeelingState NewFeelingState = FeelingState;
	for (const auto& FeelingStateValueMapping : FeelingSettings->FeelingStateActivationThresholds)
	{
		if (NormalizedFeel < FeelingStateValueMapping.Threshold)
		{
			NewFeelingState = FeelingStateValueMapping.State;
			break;
		}
	}

	if (NewFeelingState != FeelingState)
	{
		FeelingState = NewFeelingState;
		OwnerController->GetBlackboardComponent()->SetValueAsEnum(BBKey, (uint8)NewFeelingState);
	}
}

bool UAICitizenReactionsComponent::CanApplyPropagationFromActor(ABaseCharacter* ObservedCharacter)
{
	float WorldTime = GetWorld()->GetTimeSeconds();
	FFeelingPropagationHistory* LastActorPropagation = nullptr;
	int i = LastPropagationHistoryIndex;
	do
	{
		if (PropagationHistory[i].Propagator == ObservedCharacter)
		{
			LastActorPropagation = &PropagationHistory[i];
			break;
		}
			
		i--;
		if (i < 0)
			i = PropagationHistory.Num() - 1;
	}
	while (i != LastPropagationHistoryIndex);
		
	return !LastActorPropagation || WorldTime > LastActorPropagation->ExpiresAt;
}

void UAICitizenReactionsComponent::CalmDown(float RawAnxietyChange)
{
	Anxiety = FMath::Clamp(Anxiety - RawAnxietyChange * AnxietySettings->MaxFeelLevel, 0.f, AnxietySettings->MaxFeelLevel);
	AnxietyHoldTime *= 0.1f;
	UpdateBlackboardFeelingState(Anxiety, AnxietySettings, AnxietyState, BB_AnxietyState);
	OwnerController->GetBlackboardComponent()->SetValueAsFloat(BB_Anxiety, Anxiety);
}
