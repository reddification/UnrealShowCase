#include "BaseInventoryItem.h"

void UBaseInventoryItem::Init(const FDataTableRowHandle& InitItemDTRH)
{
	this->ItemDTRH = InitItemDTRH;
}
