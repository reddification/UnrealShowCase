#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Soakable.generated.h"

UINTERFACE()
class USoakable : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API ISoakable
{
	GENERATED_BODY()

public:
	virtual void StartSoaking() {}
	virtual void StopSoaking() {}
};
