#pragma once

#include "CoreMinimal.h"
#include "QuestCustomActionBase.generated.h"

UCLASS(Abstract, Blueprintable)
class ALUMCOCKSGJ_API AQuestCustomActionBase : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void ExecuteCustomAction();
	virtual void ExecuteCustomAction_Implementation() {}
};
