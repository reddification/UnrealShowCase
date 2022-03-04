#include "AIDogCharacter2.h"

#include "AI/Components/Character/NpcPlayerInteractionComponent.h"
#include "AI/Components/Character/NpcLogic/NpcDogLogicComponent.h"
#include "Characters/PlayerCharacter.h"

AAIDogCharacter2::AAIDogCharacter2(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 CharacterInfoWidgetComponent = CreateDefaultSubobject<UWidgetNameplateComponent>("CharacterInfoWidget");
    CharacterInfoWidgetComponent->SetupAttachment(GetMesh(), "head");
    CharacterInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    CharacterInfoWidgetComponent->SetDrawAtDesiredSize(true);
    CharacterInfoWidgetComponent->CastShadow = false;

    NpcPlayerInteractionComponent = CreateDefaultSubobject<UNpcPlayerInteractionComponent>(TEXT("Npc Interaction"));
    AddOwnedComponent(NpcPlayerInteractionComponent);

    NpcLogicComponent = CreateDefaultSubobject<UNpcDogLogicComponent>(TEXT("Npc Logic"));
    AddOwnedComponent(NpcLogicComponent);
}

bool AAIDogCharacter2::SetDesiredRotation(const FRotator& Rotator)
{
    return Super::SetDesiredRotation(Rotator);
}

void AAIDogCharacter2::BeginPlay()
{
    Super::BeginPlay();
    InitializeNPC();
}

void AAIDogCharacter2::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    InitializeNPC();
}

bool AAIDogCharacter2::InteractWithPlayer(APlayerCharacter* Interactor)
{
    return NpcPlayerInteractionComponent->StartInteractWithCharacter(Interactor);
}

void AAIDogCharacter2::SetNpcDTRH(const FDataTableRowHandle& NpcDTRH)
{
    NpcLogicComponent->SetNpcDTRH(NpcDTRH);
}

void AAIDogCharacter2::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    Super::GetOwnedGameplayTags(TagContainer);
    NpcLogicComponent->GetNpcLogicTags(TagContainer);
}

bool AAIDogCharacter2::IsPreferStrafing() const
{
    return NpcLogicComponent->IsPreferStrafing() || Super::IsPreferStrafing();
    // return bAiForceStrafing || bPreferStrafingState || Super::IsPreferStrafing();
}

UCharacterInteractionOptions* AAIDogCharacter2::GetInteractionOptions() const
{
    if (InteractionOptions)
        return InteractionOptions;

    return NpcLogicComponent->GetInteractionOptions();
}

void AAIDogCharacter2::InitializeNPC_Implementation()
{
    NpcLogicComponent->InitializeNPC();
}