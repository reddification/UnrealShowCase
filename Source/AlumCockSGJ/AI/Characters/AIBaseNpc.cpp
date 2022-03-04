#include "AIBaseNpc.h"

#include "AI/Components/Character/NpcPlayerInteractionComponent.h"
#include "AI/Data/NpcDTR.h"
#include "Characters/Animations/HumanoidCharacterAnimInstance.h"
#include "Components/ObservationSourceComponent.h"
#include "Components/WidgetNameplateComponent.h"
#include "Components/Movement/BaseCharacterMovementComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"
#include "Subsystems/NpcSubsystem.h"
#include "UI/CharacterInfoWidget.h"


AAIBaseNpc::AAIBaseNpc(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    CharacterInfoWidgetComponent = CreateDefaultSubobject<UWidgetNameplateComponent>("CharacterInfoWidget");
    CharacterInfoWidgetComponent->SetupAttachment(GetMesh(), "head");
    CharacterInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    CharacterInfoWidgetComponent->SetDrawAtDesiredSize(true);
    CharacterInfoWidgetComponent->CastShadow = false;

    NpcPlayerInteractionComponent = CreateDefaultSubobject<UNpcPlayerInteractionComponent>(TEXT("Npc Interaction"));
    AddOwnedComponent(NpcPlayerInteractionComponent);
}

void AAIBaseNpc::ApplyState(const UBaseNpcStateSettings* NpcState)
{
    BaseMovementComponent->SetForcedWalkSpeed(NpcState->MovementWalkSpeed);
    auto AnimInstance = Cast<UHumanoidCharacterAnimInstance>(GetMesh()->GetAnimInstance());
    AnimInstance->ApplyState(NpcState);
    bPreferStrafingState = NpcState->bStrafing;
    UpdateStrafingControls();
}

void AAIBaseNpc::BeginPlay()
{
    Super::BeginPlay();
    check(CharacterInfoWidgetComponent);
    CharacterAttributesComponent->AttributeChangedEvent.AddUObject(this, &AAIBaseNpc::OnAttributeChanged);
    
    GetComponents<UObservationSourceComponent>(ObservationSourceComponents);
    for (auto ObservationSourceComponent : ObservationSourceComponents)
        ObservationSourceComponent->SightStateChangedEvent.AddUObject(this, &AAIBaseNpc::OnSightStateChanged);
    
    InitializeNPC();
    RegisterNpcSubsystem();

    if (!bAiEnabled)
    {
        UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
        AIPerceptionSys->UnregisterSource(*this);
    }
}

void AAIBaseNpc::RegisterNpcSubsystem()
{
    if (!NpcDTRH.IsNull())
    {
        auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
        NpcSubsystem->RegisterNpc(NpcDTRH, this);
    }
}

void AAIBaseNpc::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    InitializeNPC();
}

// TODO code clone with AIOutlawCharacter. Refactor
void AAIBaseNpc::InitializeNPC_Implementation()
{
    if (NpcDTRH.IsNull())
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

const FGameplayTag& AAIBaseNpc::GetState() const
{
    return CharacterStateTag;
}

bool AAIBaseNpc::TrySetState(const FGameplayTag& GameplayTag)
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

bool AAIBaseNpc::InteractWithPlayer(APlayerCharacter* Interactor)
{
    return NpcPlayerInteractionComponent->StartInteractWithCharacter(Interactor);
}

void AAIBaseNpc::SetNpcDTRH(const FDataTableRowHandle& InNpcDTRH)
{
    NpcDTRH = InNpcDTRH;
    InitializeNPC();
    RegisterNpcSubsystem();
}

void AAIBaseNpc::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    Super::GetOwnedGameplayTags(TagContainer);
    TagContainer.AddTagFast(CharacterStateTag);
}

void AAIBaseNpc::SetAiForcedStrafing(bool bNewState)
{
    FGameplayTagContainer CurrentCharacterTags;
    GetOwnedGameplayTags(CurrentCharacterTags);
    if (bNewState && IgnoreAiForcedStrafeInStates.Matches(CurrentCharacterTags))
        return;
    
    bAiForceStrafing = bNewState;
    UpdateStrafingControls();
}

void AAIBaseNpc::ReactToDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    Super::ReactToDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
    if (!CharacterAttributesComponent->IsAlive())
    {
        for (auto ObservationSourceComponent : ObservationSourceComponents)
            ObservationSourceComponent->SetDisabled();

        CharacterInfoWidgetComponent->SetVisibility(false);
        return;
    }

    UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, Damage,
        DamageCauser->GetActorLocation(), GetActorLocation());
}

bool AAIBaseNpc::IsPreferStrafing() const
{
    return bAiForceStrafing || bPreferStrafingState || Super::IsPreferStrafing();
}

UCharacterInteractionOptions* AAIBaseNpc::GetInteractionOptions() const
{
    if (InteractionOptions)
        return InteractionOptions;
	
    if (NpcDTRH.IsNull())
        return nullptr;

    FNpcDTR* NpcDTR = NpcDTRH.GetRow<FNpcDTR>("");
    if (!NpcDTR)
        return nullptr;

    return NpcDTR->CharacterInteractionOptions;
}

void AAIBaseNpc::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
    if (Attribute == ECharacterAttribute::Health)
    {
        CharacterInfoWidget->SetHealthPercent(Percent);
    }
}

void AAIBaseNpc::OnSightStateChanged(bool bInSight)
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

void AAIBaseNpc::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
    auto SocketTransform = GetMesh()->GetSocketTransform(SocketEyesViewpoint);
    OutLocation = SocketTransform.GetLocation();
    OutRotation = SocketTransform.Rotator();
}

void AAIBaseNpc::OnOutOfHealth()
{
    Super::OnOutOfHealth();
    // Team = ETeam::Dead;
    // if (NpcDiedEvent.IsBound())
    //     NpcDiedEvent.Broadcast(NpcDTRH, this);
}

bool AAIBaseNpc::StartReaction(const FGameplayTag& GameplayTag, int ReactionIndex)
{
    // if (ActiveReactionTag != FGameplayTag::EmptyTag)
    // {
    //     if (NpcReactionStateChangedEvent.IsBound())
    //         NpcReactionStateChangedEvent.Broadcast(this, ActiveReactionTag, false);
    // }
    //
    // ActiveReactionTag = GameplayTag;
    // ActiveReactionIndex = ReactionIndex;
    // if (NpcReactionStateChangedEvent.IsBound())
    //     NpcReactionStateChangedEvent.Broadcast(this, GameplayTag, true);
    //
    return true;
}

bool AAIBaseNpc::StopReaction(const FGameplayTag& GameplayTag)
{
    // if (ActiveReactionTag == GameplayTag)
    // {
    //     if (NpcReactionStateChangedEvent.IsBound())
    //         NpcReactionStateChangedEvent.Broadcast(this, GameplayTag, false);
    //     
    //     ActiveReactionTag = FGameplayTag::EmptyTag;
    //     return true;
    // }

    return false;
}

bool AAIBaseNpc::IsAiEnabled() const
{
    return bAiEnabled;
}