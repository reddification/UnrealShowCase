#include "InventoryComponent.h"

bool UInventoryComponent::PickUpItem(ABasePickableItem* NewItem)
{
	AddItem(NewItem->GetItemDTR());
	NewItem->Destroy();
	return true;
}

void UInventoryComponent::AddItem(const FDataTableRowHandle& ItemDTRH)
{
	UBaseInventoryItem* InventoryItem = NewObject<UBaseInventoryItem>();
	InventoryItem->Init(ItemDTRH);
	Items.Add(InventoryItem);
	if (ItemPickedUpEvent.IsBound())
	{
		ItemPickedUpEvent.Broadcast(ItemDTRH);
	}
}

bool UInventoryComponent::HasItem(const FDataTableRowHandle& ItemDTRH) const
{
	for (const auto InventoryItem : Items)
	{
		if (InventoryItem->GetItemDTRH() == ItemDTRH)
			return true;
	}

	return false;
}

int UInventoryComponent::GetCountOfItem(const FDataTableRowHandle& ItemDTRH) const
{
	int Count = 0;
	for (const auto Item : Items)
	{
		if (Item->GetItemDTRH() == ItemDTRH)
			Count++;
	}

	return Count;
}
