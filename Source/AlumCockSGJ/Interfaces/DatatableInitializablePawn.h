#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/Interface.h"
#include "DatatableInitializablePawn.generated.h"

UINTERFACE()
class UDatatableInitializablePawn : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IDatatableInitializablePawn
{
	GENERATED_BODY()

public:
	virtual void SetNpcDTRH(const FDataTableRowHandle& NpcDTRH) {}
};
