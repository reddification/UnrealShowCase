#pragma once

#include "BaseAnimNotify.generated.h"

UCLASS(Abstract)
class ALUMCOCKSGJ_API UBaseAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
protected:
	class ABaseHumanoidCharacter* GetCharacter(AActor* Actor) const;	
};
