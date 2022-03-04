#include "AICharacter.h"

#include "NavigationInvokerComponent.h"
#include "AI/Components/AIPatrolComponent.h"
#include "Components/ObservationSourceComponent.h"
#include "Perception/AISense_Damage.h"
#include "Components/WidgetComponent.h"
#include "AI/Components/Character/AICharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/NpcSubsystem.h"
#include "UI/BaseInteractableInfoWidget.h"
#include "UI/CharacterInfoWidget.h"

AAICharacter::AAICharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UAICharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    AIMovementComponent = StaticCast<UAICharacterMovementComponent*>(GetCharacterMovement());

    AIPatrolComponent = CreateDefaultSubobject<UAIPatrolComponent>(TEXT("PatrolComponent"));
    AddOwnedComponent(AIPatrolComponent);

    CharacterInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("CharacterInfoWidget");
    CharacterInfoWidgetComponent->SetupAttachment(GetMesh());
    CharacterInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    CharacterInfoWidgetComponent->SetDrawAtDesiredSize(true);

    ObservationSourceComponent = CreateDefaultSubobject<UObservationSourceComponent>(TEXT("ObservationSource"));
    ObservationSourceComponent->SetupAttachment(GetMesh());

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = 1;
}

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();
    check(CharacterInfoWidgetComponent);
    CharacterAttributesComponent->AttributeChangedEvent.AddUObject(this, &AAICharacter::OnAttributeChanged);
    ObservationSourceComponent->SightStateChangedEvent.AddUObject(this, &AAICharacter::OnSightStateChanged);
    CharacterInfoWidgetComponent->SetVisibility(false);
    CharacterInfoWidget = Cast<UCharacterInfoWidget>(CharacterInfoWidgetComponent->GetWidget());
    if (CharacterInfoWidget.IsValid())
    {
        CharacterInfoWidget->SetName(CharacterSettings->Name);
    }
}

void AAICharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    auto AIController = Cast<AAIController>(NewController);
    if (!IsValid(AIController))
        return;

    AIController->SetGenericTeamId(FGenericTeamId((uint8)Team));
}

void AAICharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (CharacterInfoWidgetComponent->IsVisible())
    {
        auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
        FRotator NewRotation = NpcSubsystem->GetFacePlayerViewpointRotation(CharacterInfoWidgetComponent->GetComponentLocation());
        CharacterInfoWidgetComponent->SetWorldRotation(NewRotation);
    }
}

void AAICharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    Super::GetOwnedGameplayTags(TagContainer);
    TagContainer.AppendTags(GameplayTags);
}

void AAICharacter::ReactToDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    Super::ReactToDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
    if (!CharacterAttributesComponent->IsAlive())
    {
        ObservationSourceComponent->SetDisabled();
        CharacterInfoWidgetComponent->SetVisibility(false);
        return;
    }

    UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, Damage,
        DamageCauser->GetActorLocation(), GetActorLocation());
}

void AAICharacter::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
    if (Attribute == ECharacterAttribute::Health)
    {
        CharacterInfoWidget->SetHealthPercent(Percent);
    }
}

void AAICharacter::OnSightStateChanged(bool bInSight)
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