#include "AIOutlawCharacter.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/Data/NpcDTR.h"
#include "Components/ObservationSourceComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/GCGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "Subsystems/NpcSubsystem.h"
#include "UI/CharacterInfoWidget.h"

AAIOutlawCharacter::AAIOutlawCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    CharacterInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("CharacterInfoWidget");
    CharacterInfoWidgetComponent->SetupAttachment(GetMesh());
    CharacterInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    CharacterInfoWidgetComponent->SetDrawAtDesiredSize(true);
    CharacterInfoWidgetComponent->CastShadow = false;
}

void AAIOutlawCharacter::BeginPlay()
{
    Super::BeginPlay();
    check(CharacterInfoWidgetComponent);
    CharacterAttributesComponent->AttributeChangedEvent.AddUObject(this, &AAIOutlawCharacter::OnAttributeChanged);
    
    GetComponents<UObservationSourceComponent>(ObservationSourceComponents);
    for (auto ObservationSourceComponent : ObservationSourceComponents)
        ObservationSourceComponent->SightStateChangedEvent.AddUObject(this, &AAIOutlawCharacter::OnSightStateChanged);
    
    CharacterInfoWidgetComponent->SetVisibility(false);

    InitializeNPC();
}

void AAIOutlawCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    InitializeNPC();
}

void AAIOutlawCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    auto AIController = Cast<AAIController>(NewController);
    if (!IsValid(AIController))
        return;

    AIController->SetGenericTeamId(FGenericTeamId((uint8)Team));
}

void AAIOutlawCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (CharacterInfoWidgetComponent->IsVisible())
    {
        auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
        FRotator NewRotation = NpcSubsystem->GetFacePlayerViewpointRotation(CharacterInfoWidgetComponent->GetComponentLocation());
        CharacterInfoWidgetComponent->SetWorldRotation(NewRotation);
    }
}

void AAIOutlawCharacter::ReactToDamage(
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

void AAIOutlawCharacter::InitializeNPC_Implementation()
{
    if (NpcDTRH.IsNull())
        return;

    FNpcDTR* NpcDTR = NpcDTRH.DataTable->FindRow<FNpcDTR>(NpcDTRH.RowName, "");
    if (!NpcDTR)
        return;
    
    CharacterInfoWidget = Cast<UCharacterInfoWidget>(CharacterInfoWidgetComponent->GetWidget());
    if (CharacterInfoWidget.IsValid())
        CharacterInfoWidget->SetName(NpcDTR->Name);
}

void AAIOutlawCharacter::SpawnItemsOnDeath()
{
    Super::SpawnItemsOnDeath();
    FNpcDTR* NpcDTR = NpcDTRH.DataTable->FindRow<FNpcDTR>(NpcDTRH.RowName, "");
    if (!NpcDTR)
        return;

    SpawnItems(NpcDTR->SpawnItemsAfterDeath);
}

void AAIOutlawCharacter::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
    if (Attribute == ECharacterAttribute::Health)
    {
        CharacterInfoWidget->SetHealthPercent(Percent);
    }
}

void AAIOutlawCharacter::OnSightStateChanged(bool bInSight)
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

void AAIOutlawCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
    auto SocketTransform = GetMesh()->GetSocketTransform(SocketEyesViewpoint);
    OutLocation = SocketTransform.GetLocation();
    OutRotation = SocketTransform.Rotator();
}

UBehaviorTree* AAIOutlawCharacter::GetDefaultBehaviorTree() const
{
    if (NpcDTRH.IsNull())
        return nullptr;

    FNpcDTR* NpcDTR = NpcDTRH.DataTable->FindRow<FNpcDTR>(NpcDTRH.RowName, "");
    if (!NpcDTR)
        return nullptr;
    
    return NpcDTR->BaseBehavior;
}

void AAIOutlawCharacter::SetNpcDTRH(const FDataTableRowHandle& InNpcDTRH)
{
    NpcDTRH = InNpcDTRH;
    InitializeNPC();
}