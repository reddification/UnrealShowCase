#pragma once

#include "CoreMinimal.h"
#include "BasePickableItem.h"
#include "PickableConsumableItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APickableConsumableItem : public ABasePickableItem
{
	GENERATED_BODY()

protected:
	virtual bool PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = -1000.f, ClampMin = -1000.f, UIMax = 1000.f, ClampMax = 1000.f))
	float HealthGain = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = -1.f, ClampMin = -1.f, UIMax = 1.f, ClampMax = 1.f))
	float SoaknessGain = -0.2f;
};
