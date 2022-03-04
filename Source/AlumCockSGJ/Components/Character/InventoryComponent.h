#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/BasePickableItem.h"
#include "Components/ActorComponent.h"
#include "Data/Entities/BaseInventoryItem.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FItemPickedUpEvent, const FDataTableRowHandle& ItemDTR)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool PickUpItem(ABasePickableItem* NewItem);
	void AddItem(const FDataTableRowHandle& ItemDTRH);
	bool HasItem(const FDataTableRowHandle& ItemDTRH) const;
	int GetCountOfItem(const FDataTableRowHandle& ItemDTRH) const;

	mutable FItemPickedUpEvent ItemPickedUpEvent;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UBaseInventoryItem*> Items;
};
