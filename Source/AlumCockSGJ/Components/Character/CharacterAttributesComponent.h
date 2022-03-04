#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAttributesComponent.h"
#include "CharacterAttributesComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALUMCOCKSGJ_API UCharacterAttributesComponent : public UBaseCharacterAttributesComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributesComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  
	void SetSuffocating(bool bNewState);
	void OnJumped();
	virtual bool ChangeAttribute(ECharacterAttribute Attribute, float Delta) override;

protected:
	virtual void BeginPlay() override;
	virtual float GetStaminaConsumption() const override;

private:
	float Oxygen = 50.f;
	
	bool bSuffocating = false;
	void UpdateOxygen(float DeltaTime);
	
	FTimerHandle DrowningTimer;
	void DrowningCallback() const;
	
	TWeakObjectPtr<class ABaseHumanoidCharacter> CharacterOwner;
	TWeakObjectPtr<const class UHumanoidCharacterAttributesSettings> HumanoidCharacterSettings;
};
