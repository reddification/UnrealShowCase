#include "AICitizenCharacter.h"

#include "AI/Components/Character/NpcConversationComponent.h"
#include "AI/Data/NpcDTR.h"
#include "Characters/PlayerCharacter.h"
#include "AI/Components/Character/NpcPlayerInteractionComponent.h"
#include "AI/Components/Character/NpcLogic/NpcHumanoidLogicComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetNameplateComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionSystem.h"
#include "Subsystems/WorldStateSubsystem.h"

AAICitizenCharacter::AAICitizenCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    CharacterInfoWidgetComponent = CreateDefaultSubobject<UWidgetNameplateComponent>("CharacterInfoWidget");
    CharacterInfoWidgetComponent->SetupAttachment(GetMesh(), "head");
    CharacterInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    CharacterInfoWidgetComponent->SetDrawAtDesiredSize(true);
    CharacterInfoWidgetComponent->CastShadow = false;

    NpcPlayerInteractionComponent = CreateDefaultSubobject<UNpcPlayerInteractionComponent>(TEXT("Npc Interaction"));
    AddOwnedComponent(NpcPlayerInteractionComponent);

    NpcConversationComponent = CreateDefaultSubobject<UNpcConversationComponent>(TEXT("Npc Conversation"));
    AddOwnedComponent(NpcConversationComponent);

    NpcLogicComponent = CreateDefaultSubobject<UNpcHumanoidLogicComponent>(TEXT("Npc Logic"));
    AddOwnedComponent(NpcLogicComponent);
}

void AAICitizenCharacter::BeginPlay()
{
    Super::BeginPlay();
    InitializeNPC();
}

void AAICitizenCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    InitializeNPC();
}

bool AAICitizenCharacter::InteractWithPlayer(APlayerCharacter* Interactor)
{
    return NpcPlayerInteractionComponent->StartInteractWithPlayer(Interactor);
}

void AAICitizenCharacter::SetNpcDTRH(const FDataTableRowHandle& NpcDTRH)
{
    NpcLogicComponent->SetNpcDTRH(NpcDTRH);
}

void AAICitizenCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    Super::GetOwnedGameplayTags(TagContainer);
    NpcLogicComponent->GetNpcLogicTags(TagContainer);
}

bool AAICitizenCharacter::IsPreferStrafing() const
{
    return NpcLogicComponent->IsPreferStrafing() || Super::IsPreferStrafing();
    // return bAiForceStrafing || bPreferStrafingState || Super::IsPreferStrafing();
}

UCharacterInteractionOptions* AAICitizenCharacter::GetInteractionOptions() const
{
    if (InteractionOptions)
        return InteractionOptions;

    return NpcLogicComponent->GetInteractionOptions();
}

void AAICitizenCharacter::InitializeNPC_Implementation()
{
    NpcLogicComponent->InitializeNPC();
}