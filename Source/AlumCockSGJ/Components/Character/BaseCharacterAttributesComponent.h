#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/CharacterTypes.h"
#include "Data/DataAssets/Components/BaseAttributesSettings.h"
#include "SaveSystem/SaveSubsystemInterface.h"
#include "BaseCharacterAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOutOfHealthEvent);
DECLARE_MULTICAST_DELEGATE_TwoParams(FAttributeChangedEvent, ECharacterAttribute Attribute, float Percent);
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaEvent, bool bOutOfStamina);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UBaseCharacterAttributesComponent : public UActorComponent, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	mutable FAttributeChangedEvent AttributeChangedEvent;
	mutable FOutOfHealthEvent OutOfHealthEvent;

	bool TryAddHealth(float AddHealthValue);
	virtual void OnLevelDeserialized_Implementation() override;
	bool IsAlive() const { return Health > 0.f; }

	virtual bool ChangeAttribute(ECharacterAttribute Attribute, float Delta);
	
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	float GetStamina() const { return Stamina; }
	float GetStaminaNormalized() const { return Stamina / AttributesSettings->MaxStamina; }
	bool IsOutOfStamina() const { return bRegeneratingStamina; }
	void ChangeStaminaValue(float StaminaModification);
	float GetHealth() const { return Health; }
	mutable FOutOfStaminaEvent OutOfStaminaEvent;
	
protected:
	virtual void BeginPlay() override;

	float Health = 0.f;
	float Stamina = 0.f;
	bool bRegeneratingStamina = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class UBaseAttributesSettings* AttributesSettings;
	
	UPROPERTY(SaveGame)
	float CurrentHealth = 0;

	bool TryChangeHealth(float Delta);

	virtual float GetStaminaConsumption() const;

private:
	TWeakObjectPtr<class ABaseCharacter> BaseCharacterOwner;
	bool CanRestoreStamina() const;
	bool IsConsumingStamina() const;
	void UpdateStamina(float DeltaTime);
};
