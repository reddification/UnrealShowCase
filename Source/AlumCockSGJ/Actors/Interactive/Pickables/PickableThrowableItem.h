#pragma once

#include "CoreMinimal.h"
#include "BasePickableItem.h"
#include "GameFramework/Actor.h"
#include "PickableThrowableItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APickableThrowableItem : public ABasePickableItem
{
	GENERATED_BODY()

protected:
	virtual bool PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AThrowableItem> ThrowableItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AddSupply = 2;
};
