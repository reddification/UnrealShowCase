#include "Actors/Equipment/EquippableItem.h"

#include "Characters/BaseCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Data/Entities/WorldItemDTR.h"

AEquippableItem::AEquippableItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AEquippableItem::BeginPlay()
{
	Super::BeginPlay();
	checkf(IsValid(ItemSettings), TEXT("Item settings DataAsset must be set"));	
}

bool AEquippableItem::TryAddToLoadout(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpSettings)
{
     return true;
}

const FWeaponDTR* AEquippableItem::GetWeaponDTR() const
{
	return EquipableItemDTRH.IsNull() || EquipableItemDTRH.RowName.IsNone() ? nullptr : EquipableItemDTRH.GetRow<FWeaponDTR>("");
}

bool AEquippableItem::TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpSettings)
{
	bool bAddedToLoadout = TryAddToLoadout(EquipmentComponent, PickUpSettings);
	if (!bAddedToLoadout)
		return false;
	
	SetOwner(EquipmentComponent->GetOwner());
	AttachToComponent(EquipmentComponent->CharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ItemSettings->CharacterUnequippedSocketName);
    return true;
}

void AEquippableItem::OnEquipped(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
	AttachToComponent(CharacterEquipmentComponent->CharacterOwner->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform, ItemSettings->CharacterEquippedSocketName);
}

bool AEquippableItem::CanBePickedUp(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
	return !IsValid(CharacterEquipmentComponent->Loadout[(uint8)ItemSettings->DesignatedSlot]);
}

const FText& AEquippableItem::GetName()
{
	if (!EquipableItemDTRH.IsNull())
	{
		auto WeaponDTR = EquipableItemDTRH.GetRow<FWorldItemDTR>("");
		if (WeaponDTR)
			return WeaponDTR->Name;
	}
	
	return FText::GetEmpty();
}

void AEquippableItem::OnDropped(APickableEquipmentItem* PickableEquipmentItem)
{
	PickableEquipmentItem->SetDroppedState(0);
}
