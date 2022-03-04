#include "CharacterAttributesComponent.h"

#include "AIController.h"
#include "CharacterEquipmentComponent.h"
#include "DrawDebugHelpers.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Data/EnvironmentDamage.h"
#include "Data/DataAssets/Components/BaseAttributesSettings.h"
#include "Data/DataAssets/Components/HumanoidCharacterAttributesSettings.h"
#include "Kismet/GameplayStatics.h"

UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	checkf(IsValid(AttributesSettings) && AttributesSettings->IsA<UHumanoidCharacterAttributesSettings>(),
		TEXT("This character settings must be a valid HumanoidCharacterAttributesDataAsset"))
	CharacterOwner = StaticCast<ABaseHumanoidCharacter*>(Owner);
	HumanoidCharacterSettings = StaticCast<const UHumanoidCharacterAttributesSettings*>(AttributesSettings);
	Oxygen = HumanoidCharacterSettings->MaxOxygen;
	Stamina = HumanoidCharacterSettings->MaxStamina;
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Health > 0.f)
	{
		UpdateOxygen(DeltaTime);
	}
}

float UCharacterAttributesComponent::GetStaminaConsumption() const
{
	return Super::GetStaminaConsumption() + CharacterOwner->GetGCMovementComponent()->IsSprinting() || CharacterOwner->GetGCMovementComponent()->IsWallrunning()
		? HumanoidCharacterSettings->SprintStaminaConsumptionRate
		: 0.f;
}

void UCharacterAttributesComponent::UpdateOxygen(float DeltaTime)
{
	bool bOxygenUpdated = false;
	if (bSuffocating)
	{
		if (Oxygen > 0.f)
		{
			Oxygen = FMath::Max(Oxygen - HumanoidCharacterSettings->OxygenConsumptionRate * DeltaTime, 0.f);
			bOxygenUpdated = true;
		}
		else if (!GetWorld()->GetTimerManager().IsTimerActive(DrowningTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(DrowningTimer, this, &UCharacterAttributesComponent::DrowningCallback,
				HumanoidCharacterSettings->DrowningHealthDamageInterval, true);
		}
	}
	else if (Oxygen < HumanoidCharacterSettings->MaxOxygen)
	{
		Oxygen = FMath::Min(Oxygen + HumanoidCharacterSettings->OxygenRestoreRate * DeltaTime, HumanoidCharacterSettings->MaxOxygen);
		bOxygenUpdated = true;
	}

	if (bOxygenUpdated && AttributeChangedEvent.IsBound())
	{
		AttributeChangedEvent.Broadcast(ECharacterAttribute::Oxygen, Oxygen / HumanoidCharacterSettings->MaxOxygen);
	}
}

void UCharacterAttributesComponent::DrowningCallback() const
{
	// Could have called OnTakeAnyDamage directly but still doing it through the character
	// just in case any other entities subscribed to AActor::OnTakeAnyDamage of the character
	GetOwner()->TakeDamage(HumanoidCharacterSettings->DrowningHealthDamage, GetEnvironmentDamageEvent(),
		CharacterOwner->GetController(), GetOwner());
}

bool UCharacterAttributesComponent::ChangeAttribute(ECharacterAttribute Attribute, float Delta)
{
	if (Super::ChangeAttribute(Attribute, Delta))
		return true;
	
	switch (Attribute)
	{
		case ECharacterAttribute::Stamina:
			ChangeStaminaValue(Delta);
			return true;
		// oxygen is never updated manually, only suffocation state
		case ECharacterAttribute::Oxygen:
		default:
			return false;
	}
}

void UCharacterAttributesComponent::OnJumped()
{
	ChangeStaminaValue(-HumanoidCharacterSettings->JumpStaminaConsumption);
}

void UCharacterAttributesComponent::SetSuffocating(bool bNewState)
{
	if (!IsAlive())
	{
		return;
	}
	
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	bSuffocating = bNewState;
	if (!bSuffocating && TimerManager.IsTimerActive(DrowningTimer))
	{
		TimerManager.ClearTimer(DrowningTimer);
	}
}

