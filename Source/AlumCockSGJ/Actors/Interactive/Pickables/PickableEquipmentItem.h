#pragma once

#include "CoreMinimal.h"
#include "BasePickableItem.h"
#include "Data/Entities/WorldItemDTR.h"
#include "PickableEquipmentItem.generated.h"

struct FPickUpItemData
{
	int InitialAmmo = 0;
	FVector PickUpLocation;
	bool bForce = false;
	bool bCreatingLoadout = false;
	bool bDropped = false;
};

UCLASS()
class ALUMCOCKSGJ_API APickableEquipmentItem : public ABasePickableItem
{
	GENERATED_BODY()

public:
	virtual bool InteractWithPlayer(APlayerCharacter* Interactor) override;
	void SetDroppedState(int Ammo);
		
protected:
	virtual bool PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce = false) override;

private:
	bool bDropped = false;
	int InitialAmmo = 0;
	bool bResupplied = false;

	bool TryAddSupply(class UCharacterEquipmentComponent* EquipmentComponent,
	                  const TSubclassOf<class AEquippableItem>& ItemClass, int Quantity, EItemType ItemType);
};
