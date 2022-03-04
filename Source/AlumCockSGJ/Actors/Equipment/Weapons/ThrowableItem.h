#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquippableItem.h"
#include "Data/DataAssets/Items/ThrowableItemSettings.h"
#include "ThrowableItem.generated.h"

class AProjectile;
class ABaseThrowableProjectile;
UCLASS(Blueprintable)
class ALUMCOCKSGJ_API AThrowableItem : public AEquippableItem
{
	GENERATED_BODY()

public:
	void Throw(AController* OwnerController);

	// TODO just expose getter for DataAsset? 
	const TSubclassOf<ABaseThrowableProjectile>& GetProjectileClass() const { return ThrowableItemSettings->ProjectileClass; }
	UAnimMontage* GetThrowMontage() const { return ThrowableItemSettings->ThrowMontage; }
	float GetThrowDuration() const { return ThrowableItemSettings->ThrowDuration; }
	virtual EAmmunitionType GetAmmoType() const override { return GetThrowableItemSettings()->AmmunitionType; }

	void Activate(AController* Controller);
	ABaseThrowableProjectile* GetCurrentProjectile();
	ABaseThrowableProjectile* SpawnProjectile();
	void DropProjectile(AController* Controller);
	
	void SpawnAttachedProjectile();
	void DestroyAttachedProjectile();

	const UThrowableItemSettings* GetThrowableItemSettings() const;
	virtual bool CanBePickedUp(UCharacterEquipmentComponent* CharacterEquipmentComponent) override;
	
protected:
	virtual void BeginPlay() override;
	virtual bool TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData&) override;
	virtual bool TryAddToLoadout(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpSettings) override;
	virtual void Destroyed() override;
	
private:
	TWeakObjectPtr<ABaseThrowableProjectile> AttachedProjectile;
	TWeakObjectPtr<ABaseThrowableProjectile> CurrentProjectile;
	TWeakObjectPtr<const class UThrowableItemSettings> ThrowableItemSettings;
};
