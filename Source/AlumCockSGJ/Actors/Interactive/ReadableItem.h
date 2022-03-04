#pragma once

#include "CoreMinimal.h"
#include "BaseWorldItem.h"
#include "Interfaces/Interactable.h"
#include "ReadableItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AReadableItem : public ABaseWorldItem
{
	GENERATED_BODY()

public:
	AReadableItem();

	virtual bool InteractWithPlayer(APlayerCharacter* Interactor) override;
	
protected:
	virtual void InitializeWorldItem() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UQuestGiverComponent* QuestGiverComponent;
};
