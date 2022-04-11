#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableEquipmentItem.h"
#include "Data/EquipmentTypes.h"
#include "Data/UserInterfaceTypes.h"
#include "Data/DataAssets/Items/EquippableItemSettings.h"
#include "Data/Entities/WeaponDTR.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "EquippableItem.generated.h"

class UAnimMontage;
class UNameplateComponent;

UCLASS(Abstract, NotBlueprintable)
class ALUMCOCKSGJ_API AEquippableItem : public AActor
{
	GENERATED_BODY()

public:
	AEquippableItem();
	
	EEquippableItemType GetEquippableItemType() const { return ItemSettings->EquippableItemType; }

	FName GetCharacterEquippedSocketName() const { return ItemSettings->CharacterEquippedSocketName; }
	FName GetCharacterUnequippedSocketName() const { return ItemSettings->CharacterUnequippedSocketName; }

	UAnimMontage* GetCharacterEquipMontage() const { return ItemSettings->CharacterEquipMontage; }
	UAnimMontage* GetCharacterUnequipMontage() const { return ItemSettings->CharacterUnequipMontage; }
	float GetEquipmentDuration() const { return ItemSettings->EquipmentDuration; }

	virtual const UEquippableItemSettings* GetSettings() { return ItemSettings; }
	
	virtual EReticleType GetReticleType() const { return ItemSettings->ReticleType; }

	const FText& GetName();

	virtual void OnDropped(UCharacterEquipmentComponent* EquipmentComponent, APickableEquipmentItem* PickableEquipmentItem);

	virtual EAmmunitionType GetAmmoType() const { return EAmmunitionType::None; }
	
	virtual bool TryAddToEquipment(class UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpItemData);	
	virtual void OnEquipped(UCharacterEquipmentComponent* CharacterEquipmentComponent);
	virtual void OnUnequipped(UCharacterEquipmentComponent* CharacterEquipmentComponent) {}
	virtual bool CanBePickedUp(UCharacterEquipmentComponent* CharacterEquipmentComponent);

	bool IsPreferStrafing() const { return ItemSettings->bPreferStrafing; }
	float GetUnequipDuration() const { return ItemSettings->UnequipDuration; }
    virtual bool TryAddToLoadout(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpSettings);

	const FWeaponDTR* GetWeaponDTR() const;
	const FDataTableRowHandle& GetWeaponDTRH() const { return EquipableItemDTRH; }
	EEquipmentSlot GetDesignatedSlot() const { return ItemSettings ? ItemSettings->DesignatedSlot : EEquipmentSlot::None; }

	virtual void SetOwner(AActor* NewOwner) override;
	virtual void RegisterOnClient(UCharacterEquipmentComponent* EquipmentComponent);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class UEquippableItemSettings* ItemSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="WeaponDTR"))
	FDataTableRowHandle EquipableItemDTRH;

	TObjectPtr<class ABaseCharacter> CharacterOwner;
};
