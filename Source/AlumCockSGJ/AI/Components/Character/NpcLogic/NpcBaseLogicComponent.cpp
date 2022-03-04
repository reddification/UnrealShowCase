#include "NpcBaseLogicComponent.h"

#include "Characters/BaseCharacter.h"
#include "Characters/Animations/HumanoidCharacterAnimInstance.h"
#include "Components/ObservationSourceComponent.h"
#include "Components/Movement/BaseCharacterMovementComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"
#include "Subsystems/NpcSubsystem.h"
#include "UI/CharacterInfoWidget.h"

void UNpcBaseLogicComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
    OwnerCharacter->CharacterAttributesComponent->AttributeChangedEvent.AddUObject(this, &UNpcBaseLogicComponent::OnAttributeChanged);
    
    OwnerCharacter->GetComponents<UObservationSourceComponent>(ObservationSourceComponents);
    for (auto ObservationSourceComponent : ObservationSourceComponents)
        ObservationSourceComponent->SightStateChangedEvent.AddUObject(this, &UNpcBaseLogicComponent::OnSightStateChanged);

    auto RawNameplateWidgetComponent = OwnerCharacter->GetComponentByClass(UWidgetNameplateComponent::StaticClass());
    if (RawNameplateWidgetComponent)
        CharacterInfoWidgetComponent = StaticCast<UWidgetNameplateComponent*>(RawNameplateWidgetComponent);
    
    OwnerCharacter->CharacterDeathEvent.AddUObject(this, &UNpcBaseLogicComponent::OnDeath);
    OwnerCharacter->OnTakeAnyDamage.AddDynamic(this, &UNpcBaseLogicComponent::ReactToDamage);
    
    InitializeNPC();
    RegisterNpcSubsystem();

    if (!bAiEnabled)
    {
        UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
        AIPerceptionSys->UnregisterSource(*OwnerCharacter);   
    }
}

void UNpcBaseLogicComponent::RegisterNpcSubsystem() const
{
    auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
    if (NpcSubsystem)
        NpcSubsystem->RegisterNpc(NpcDTRH, OwnerCharacter);
}

const FGameplayTag& UNpcBaseLogicComponent::GetState() const
{
    return CharacterStateTag;
}

bool UNpcBaseLogicComponent::TrySetState(const FGameplayTag& GameplayTag)
{
    if (!GameplayTag.IsValid())
        return false;

    if (GameplayTag == CharacterStateTag)
        return true;
    
    auto NpcDTR = NpcDTRH.GetRow<FNpcDTR>("");
    if (!NpcDTR)
        return false;

    auto NewCharacterStateSettings = NpcDTR->NpcStates.Find(GameplayTag);
    if (!NewCharacterStateSettings || !*NewCharacterStateSettings)
        return false;

    ApplyState(*NewCharacterStateSettings);
    CharacterStateTag = GameplayTag;
    return true;
}

void UNpcBaseLogicComponent::SetNpcDTRH(const FDataTableRowHandle& InNpcDTRH)
{
    NpcDTRH = InNpcDTRH;
    InitializeNPC();
    RegisterNpcSubsystem();
}

void UNpcBaseLogicComponent::GetNpcLogicTags(FGameplayTagContainer& TagContainer) const
{
    FGameplayTagContainer NpcLogicTags;
    if (CharacterStateTag.IsValid())
        NpcLogicTags.AddTagFast(CharacterStateTag);

    if (ActiveReactionTag.IsValid())
        NpcLogicTags.AddTagFast(ActiveReactionTag);

    if (!NpcLogicTags.IsEmpty())
        TagContainer.AppendTags(NpcLogicTags);
}

void UNpcBaseLogicComponent::SetAiForcedStrafing(bool bNewState)
{
    FGameplayTagContainer CurrentCharacterTags;
    OwnerCharacter->GetOwnedGameplayTags(CurrentCharacterTags);
    if (bNewState && IgnoreAiForcedStrafeInStates.Matches(CurrentCharacterTags))
        return;
    
    bAiForceStrafing = bNewState;
    OwnerCharacter->UpdateStrafingControls();
}

void UNpcBaseLogicComponent::ReactToDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (!OwnerCharacter->CharacterAttributesComponent->IsAlive())
    {
        for (auto ObservationSourceComponent : ObservationSourceComponents)
            ObservationSourceComponent->SetDisabled();

        CharacterInfoWidgetComponent->SetVisibility(false);
        return;
    }

    UAISense_Damage::ReportDamageEvent(GetWorld(), OwnerCharacter, DamageCauser, Damage,
        DamageCauser->GetActorLocation(), OwnerCharacter->GetActorLocation());
}

bool UNpcBaseLogicComponent::IsPreferStrafing() const
{
    return bAiForceStrafing || bPreferStrafingState;
}

UCharacterInteractionOptions* UNpcBaseLogicComponent::GetInteractionOptions() const
{
    if (OwnerCharacter->InteractionOptions)
        return OwnerCharacter->InteractionOptions;
	
    if (NpcDTRH.IsNull())
        return nullptr;

    FNpcDTR* NpcDTR = NpcDTRH.GetRow<FNpcDTR>("");
    if (!NpcDTR)
        return nullptr;

    return NpcDTR->CharacterInteractionOptions;
}

void UNpcBaseLogicComponent::InitializeNPC()
{
    if (!OwnerCharacter || NpcDTRH.IsNull())
        return;
 
    FNpcDTR* NpcDTR = NpcDTRH.DataTable->FindRow<FNpcDTR>(NpcDTRH.RowName, "");
    if (!NpcDTR)
        return;

    if (CharacterInfoWidgetComponent)
    {
        CharacterInfoWidget = Cast<UCharacterInfoWidget>(CharacterInfoWidgetComponent->GetWidget());
        if (CharacterInfoWidget.IsValid())
            CharacterInfoWidget->SetName(NpcDTR->Name);
    }
    
    if (NpcDTR->InitialState.IsValid())
        TrySetState(NpcDTR->InitialState);
}

void UNpcBaseLogicComponent::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
    if (Attribute == ECharacterAttribute::Health)
    {
        CharacterInfoWidget->SetHealthPercent(Percent);
    }
}

void UNpcBaseLogicComponent::OnSightStateChanged(bool bInSight)
{
    if (bInSight)
    {
        InSightCounter++;
        if (InSightCounter == 1)
            CharacterInfoWidgetComponent->SetVisibility(bInSight);
    }
    else
    {
        InSightCounter--;
        if (InSightCounter <= 0)
            CharacterInfoWidgetComponent->SetVisibility(bInSight);
    }
}

void UNpcBaseLogicComponent::OnDeath(const ABaseCharacter* Character)
{
    OwnerCharacter->Team = ETeam::Dead;
    if (NpcDiedEvent.IsBound())
        NpcDiedEvent.Broadcast(NpcDTRH, OwnerCharacter);

    OwnerCharacter->SpawnItems(SpawnItemsAfterDeath);
    auto NpcDTR = NpcDTRH.IsNull() || NpcDTRH.RowName.IsNone() ? nullptr : NpcDTRH.GetRow<FNpcDTR>("");
    if (!NpcDTR)
        return;

    OwnerCharacter->SpawnItems(NpcDTR->SpawnItemsAfterDeath);
}

void UNpcBaseLogicComponent::ApplyState(const UBaseNpcStateSettings* NpcState)
{
    OwnerCharacter->BaseMovementComponent->SetForcedWalkSpeed(NpcState->MovementWalkSpeed);
    auto AnimInstance = Cast<UBaseCharacterAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
    AnimInstance->ApplyState(NpcState);
    bPreferStrafingState = NpcState->bStrafing;
    OwnerCharacter->UpdateStrafingControls();
}

void UNpcBaseLogicComponent::SetDesiredSpeed(float DesiredSpeed)
{
    OwnerCharacter->BaseMovementComponent->SetForcedWalkSpeed(DesiredSpeed);
}

bool UNpcBaseLogicComponent::SetDesiredRotation(const FRotator& Rotation) const
{
    FGameplayTagContainer CurrentCharacterTags;
    OwnerCharacter->GetOwnedGameplayTags(CurrentCharacterTags);
    if (IgnoreAiForcedStrafeInStates.Matches(CurrentCharacterTags))
        return false;
    
    return OwnerCharacter->SetDesiredRotation(Rotation);
}

bool UNpcBaseLogicComponent::StartReaction(const FGameplayTag& GameplayTag, int ReactionIndex)
{
    if (ActiveReactionTag != FGameplayTag::EmptyTag)
    {
        if (NpcReactionStateChangedEvent.IsBound())
            NpcReactionStateChangedEvent.Broadcast(OwnerCharacter, ActiveReactionTag, false);
    }
    
    ActiveReactionTag = GameplayTag;
    ActiveReactionIndex = ReactionIndex;
    if (NpcReactionStateChangedEvent.IsBound())
        NpcReactionStateChangedEvent.Broadcast(OwnerCharacter, GameplayTag, true);
    
    return true;
}

bool UNpcBaseLogicComponent::StopReaction(const FGameplayTag& GameplayTag)
{
    if (ActiveReactionTag == GameplayTag)
    {
        if (NpcReactionStateChangedEvent.IsBound())
            NpcReactionStateChangedEvent.Broadcast(OwnerCharacter, GameplayTag, false);
        
        ActiveReactionTag = FGameplayTag::EmptyTag;
        if (ReactionAttachedItems.Num() > 0)
        {
            for (const auto& ReactionAttachedItem : ReactionAttachedItems)
                ReactionAttachedItem.Value->DestroyComponent();    
            
            ReactionAttachedItems.Reset();
        }
        return true;
    }

    return false;
}

void UNpcBaseLogicComponent::AttachReactionItem(const FGameplayTag& ItemTag, bool bAttach)
{
    if (!ActiveReactionTag.IsValid())
        return;
    
    if (bAttach)
    {
        auto NpcDTR = NpcDTRH.GetRow<FNpcDTR>("");
        auto GestureItem = NpcDTR->Reactions->NpcReactions[ActiveReactionTag].NpcReactions[ActiveReactionIndex].GestureItems.Find(ItemTag);
        if (!GestureItem)
            return;

        auto StaticMeshComponent = NewObject<UStaticMeshComponent>(OwnerCharacter);
        StaticMeshComponent->SetCollisionProfileName(ProfileNoCollision);
        StaticMeshComponent->RegisterComponent();
        StaticMeshComponent->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GestureItem->AttachmentSocket);
        StaticMeshComponent->SetStaticMesh(GestureItem->StaticMesh);
        
        auto ExistingAttachedItem = ReactionAttachedItems.Find(ItemTag);
        if (ExistingAttachedItem)
        {
            (*ExistingAttachedItem)->DestroyComponent();
            ReactionAttachedItems[ItemTag] = StaticMeshComponent;
        }
        else
        {
            ReactionAttachedItems.Add(ItemTag, StaticMeshComponent);
        }
    }
    else
    {
        bool bFound = false;
        for (const auto& AttachedItem : ReactionAttachedItems)
        {
            if (AttachedItem.Key == ItemTag)
            {
                AttachedItem.Value->DestroyComponent();
                bFound = true;
            }
        }
        
        if (bFound)
            ReactionAttachedItems.Remove(ItemTag);
    }
}

bool UNpcBaseLogicComponent::IsAiEnabled() const
{
    return bAiEnabled;
}