#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TraversalActivity.generated.h"

USTRUCT()
struct FNextLocationData
{
	GENERATED_BODY()

	FVector NextLocation = FVector::ZeroVector;
	float RecommendedStayTime = 0;
};

class AAIController;
UINTERFACE()
class UTraversalActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API ITraversalActivity
{
	GENERATED_BODY()

public:
	virtual bool FindNextLocation(const AAIController* AIController, FNextLocationData& OutNextLocationData) { return false; }
};
