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
	// auto EquipmentComponent = Interactor->GetEquipmentComponent();
	//
	// int Quantity = AddSupply;
	// auto ItemClass = EquipmentItemClass;
	// auto WorldItemDTR = ItemDTRH.DataTable->FindRow<FWorldItemDTR>(ItemDTRH.RowName, "");
	// const FWeaponDTR* WeaponDTR = ItemDTRH.GetRow<FWeaponDTR>("");
	// if (WeaponDTR)
	// {
	// 	Quantity = WeaponDTR->AddSupply;
	// 	ItemClass = WeaponDTR->EquipmentItemClass;
	// }
	//
	// bool bPickedUpSupply = TryAddSupply(EquipmentComponent, Quantity);
	// bool bPickedUp = Interactor->GetEquipmentComponent()->PickUpItem(EquipmentItemClass, true);
	//
	// if (WorldItemDTR && (bPickedUp || bPickedUpSupply))
	// 	PlayPickUpSound(WorldItemDTR);
	//
	// if (bPickedUp)
	// 	Destroy();
	//
	// return true;
}

void APickableEquipmentItem::SetDroppedState(int Ammo)
{
	InitialAmmo = Ammo;
	bResupplied = true;
	bDropped = true;
}

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
	bool bPickedUp = false;
	switch (WeaponDTR->ItemType)
	{
		case EItemType::Throwables:
			bPickedUp = EquipmentComponent->PickUpThrowable(ItemClass, PickUpSettings);
			break;
		default:
			bPickedUp = EquipmentComponent->PickUpItem(ItemClass, PickUpSettings);
		break;
	}
	
	bool bPickedUpSupply = TryAddSupply(EquipmentComponent, ItemClass, Quantity);
	if (bPickedUpSupply || bPickedUp)
		PlayPickUpSound(WorldItemDTR);

	if (bPickedUp || (bPickedUpSupply && WeaponDTR->ItemType == EItemType::Throwables))
		Destroy();

	return bPickedUp;
}

bool APickableEquipmentItem::TryAddSupply(UCharacterEquipmentComponent* EquipmentComponent,
	const TSubclassOf<AEquippableItem>& ItemClass, int Quantity)
{
	if (bResupplied)
		return false;

	auto DefaultObject = ItemClass.GetDefaultObject();
	auto AmmoType = DefaultObject->GetAmmoType();		
	bResupplied = AmmoType != EAmmunitionType::None ? EquipmentComponent->PickUpAmmo(AmmoType, Quantity) : false;
	return bResupplied;
}
