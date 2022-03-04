#pragma once

#include "CoreMinimal.h"
#include "EnvironmentDamage.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UEnvironmentDamage : public UDamageType
{
	GENERATED_BODY()

public:
	UEnvironmentDamage(const FObjectInitializer& ObjectInitializer);	
};

FDamageEvent GetEnvironmentDamageEvent();