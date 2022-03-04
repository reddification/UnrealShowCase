#pragma once

#include "CoreMinimal.h"
#include "NpcBaseLogicComponent.h"
#include "NpcDogLogicComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UNpcDogLogicComponent : public UNpcBaseLogicComponent
{
	GENERATED_BODY()

public:
	virtual void ApplyState(const UBaseNpcStateSettings* NpcState) override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	class AAIDogCharacter2* DogCharacter;
};
