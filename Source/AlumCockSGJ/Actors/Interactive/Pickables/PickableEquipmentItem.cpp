#include "PickableEquipmentItem.h"
#include "Actors/Equipment/EquippableItem.h"
#include "Characters/PlayerCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Data/Entities/WeaponDTR.h"

bool APickableEquipmentItem::InteractWithPlayer(APlayerCharacter* Interactor)
{
	if (ItemDTRH.IsNull())
		return false;

	return PickUp(Interactor, ItemDTRH.GetRow<FWorldItemDTR>(""), true);
}

void APickableEquipmentItem::SetDroppedState(int Ammo)
{
	InitialAmmo = Ammo;
	bResupplied = true;
	bDropped = true;
}

// TODO refactor throwables
bool APickableEquipmentItem::PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce)
{
	Super::PickUp(PlayerCharacter, WorldItemDTR, bForce);
	auto EquipmentComponent = PlayerCharacter->GetEquipmentComponent();
	// order matters for throwables
	FWeaponDTR* WeaponDTR = ItemDTRH.GetRow<FWeaponDTR>("");
	if (!WeaponDTR)
		return false;
	
	int Quantity = WeaponDTR->AddSupply;
	auto ItemClass = WeaponDTR->EquipmentItemClass;

	FPickUpItemData PickUpSettings;
	PickUpSettings.bForce = bForce;
	PickUpSettings.InitialAmmo = InitialAmmo;
	PickUpSettings.PickUpLocation = GetActorLocation();
	PickUpSettings.bDropped = bDropped;
	if (WeaponDTR->ItemType == EItemType::Throwables && bDropped)
		Quantity = InitialAmmo;

	bool bPickedUp = EquipmentComponent->PickUpItem(ItemClass, PickUpSettings);
	bool bPickedUpSupply = TryAddSupply(EquipmentComponent, ItemClass, Quantity, WeaponDTR->ItemType);
	if (bPickedUpSupply || bPickedUp)
		PlayPickUpSound(WorldItemDTR);

	if (bPickedUp || bPickedUpSupply && WeaponDTR->ItemType == EItemType::Throwables)
		Destroy();

	return bPickedUp;
}

bool APickableEquipmentItem::TryAddSupply(UCharacterEquipmentComponent* EquipmentComponent,
                                          const TSubclassOf<AEquippableItem>& ItemClass, int Quantity, EItemType ItemType)
{
	bool bThrowable = ItemType == EItemType::Throwables;
	if (bResupplied && !bThrowable)
		return false;

	auto DefaultObject = ItemClass.GetDefaultObject();
	auto AmmoType = DefaultObject->GetAmmoType();		
	bResupplied = AmmoType != EAmmunitionType::None
		? EquipmentComponent->PickUpAmmo(AmmoType, Quantity, bThrowable)
		: false;
	
	return bResupplied;
}
