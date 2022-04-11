#include "BaseCharacterAttributesComponent.h"

#include "Characters/BaseCharacter.h"
#include "Data/DataAssets/Components/BaseAttributesSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interfaces/KillConsiderate.h"
#include "Net/UnrealNetwork.h"

UBaseCharacterAttributesComponent::UBaseCharacterAttributesComponent()
{
    SetIsReplicatedByDefault(true);
}

void UBaseCharacterAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBaseCharacterAttributesComponent, Health)
}

void UBaseCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick Tick, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, Tick, ThisTickFunction);
    if (Health > 0.f)
    {
        UpdateStamina(DeltaTime);
    }
}

bool UBaseCharacterAttributesComponent::TryAddHealth(float AddHealthValue)
{
    return TryChangeHealth(AddHealthValue);
}

void UBaseCharacterAttributesComponent::BeginPlay()
{
    Super::BeginPlay();
    checkf(IsValid(AttributesSettings), TEXT("AttributesSettings must be set!"));
    Health = AttributesSettings->MaxHealth;
    CurrentHealth = AttributesSettings->MaxHealth;
    BaseCharacterOwner = Cast<ABaseCharacter>(GetOwner());
}

bool UBaseCharacterAttributesComponent::TryChangeHealth(float Delta)
{
    float NewHealth = FMath::Clamp(Health + Delta, 0.f, AttributesSettings->MaxHealth);
    if (NewHealth == Health)
        return false;

    Health = NewHealth;
    CurrentHealth = Health;
    if (AttributeChangedEvent.IsBound())
        AttributeChangedEvent.Broadcast(ECharacterAttribute::Health, Health / AttributesSettings->MaxHealth);
    
    return true;
}

bool UBaseCharacterAttributesComponent::ChangeAttribute(ECharacterAttribute Attribute, float Delta)
{
    switch (Attribute)
    {
    case ECharacterAttribute::Health:
        TryChangeHealth(Delta);
        return true;
    default:
        return false;
    }
}

void UBaseCharacterAttributesComponent::OnTakeAnyDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (!IsAlive())
    {
        return;
    }

    auto DamageCauserCharacter = Cast<ABaseCharacter>(DamageCauser);
    if (!DamageType || !DamageType->bCausedByWorld)
        if (IsValid(DamageCauserCharacter) && DamageCauserCharacter->GetTeam() == BaseCharacterOwner->GetTeam()
            && !AttributesSettings->bReceiveTeamDamage)
        return;

    if (!AttributesSettings->bReceivesDamage)
        return;

    TryChangeHealth(-Damage);
    if (Health <= 0.f && OutOfHealthEvent.IsBound())
    {
        if (IsValid(InstigatedBy) && IsValid(DamagedActor))
        {
            // Maybe controller isn't the best option. Especially for multiplayer
            IKillConsiderate* ReportKill = Cast<IKillConsiderate>(InstigatedBy);
            if (ReportKill)
            {
                ReportKill->ReportKill(DamagedActor);
            }
        }

        // if (IsValid(DamageCauserCharacter))
        // 	DamageCauserCharacter->ReportKill(DamagedActor);

        OutOfHealthEvent.Broadcast();
    }
}

void UBaseCharacterAttributesComponent::OnLevelDeserialized_Implementation()
{
    Health = CurrentHealth;
    // ChangeHealth(-CurrentHealth);
    if (AttributeChangedEvent.IsBound())
    {
        AttributeChangedEvent.Broadcast(ECharacterAttribute::Health, Health / AttributesSettings->MaxHealth);
    }

    if (Health <= 0.f && OutOfHealthEvent.IsBound())
    {
        OutOfHealthEvent.Broadcast();
    }
}

bool UBaseCharacterAttributesComponent::CanRestoreStamina() const
{
    return !BaseCharacterOwner->GetMovementComponent()->IsFalling() && !IsConsumingStamina();
}

bool UBaseCharacterAttributesComponent::IsConsumingStamina() const
{
    EMovementMode MovementMode = BaseCharacterOwner->GetCharacterMovement()->MovementMode;
    return (MovementMode != EMovementMode::MOVE_Walking && MovementMode != EMovementMode::MOVE_NavWalking) ||
           BaseCharacterOwner->IsConsumingStamina();
}

void UBaseCharacterAttributesComponent::UpdateStamina(float DeltaTime)
{
    const float StaminaConsumption = GetStaminaConsumption();
    if (StaminaConsumption > 0.f)
    {
        ChangeStaminaValue(-StaminaConsumption * DeltaTime);
    }
    else if (CanRestoreStamina() && Stamina != AttributesSettings->MaxStamina)
    {
        ChangeStaminaValue(AttributesSettings->StaminaRestoreVelocity * DeltaTime);
    }
}

float UBaseCharacterAttributesComponent::GetStaminaConsumption() const
{
    return 0.f;
}

void UBaseCharacterAttributesComponent::OnRep_Health(float OldHealth)
{
    AttributeChangedEvent.Broadcast(ECharacterAttribute::Health, Health / AttributesSettings->MaxHealth);
    if (Health <= 0.f && OutOfHealthEvent.IsBound())
        OutOfHealthEvent.Broadcast();
}

void UBaseCharacterAttributesComponent::ChangeStaminaValue(float StaminaModification)
{
    if (FMath::IsNearlyZero(StaminaModification))
    {
        return;
    }

    Stamina = FMath::Clamp(Stamina + StaminaModification, 0.f, AttributesSettings->MaxStamina);
    if (AttributeChangedEvent.IsBound())
    {
        AttributeChangedEvent.Broadcast(ECharacterAttribute::Stamina, Stamina / AttributesSettings->MaxStamina);
    }

    if (Stamina == 0.f && !bRegeneratingStamina)
    {
        bRegeneratingStamina = true;
        if (OutOfStaminaEvent.IsBound())
        {
            OutOfStaminaEvent.Broadcast(true);
        }
    }
    else if (Stamina == AttributesSettings->MaxStamina && bRegeneratingStamina)
    {
        bRegeneratingStamina = false;
        if (OutOfStaminaEvent.IsBound())
        {
            OutOfStaminaEvent.Broadcast(false);
        }
    }
}
