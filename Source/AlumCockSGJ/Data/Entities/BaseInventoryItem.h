#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/Object.h"
#include "BaseInventoryItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBaseInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void Init(const FDataTableRowHandle& InitItemDTRH);

	const FDataTableRowHandle& GetItemDTRH() const { return ItemDTRH; }
	
protected:
	FDataTableRowHandle ItemDTRH;
};
