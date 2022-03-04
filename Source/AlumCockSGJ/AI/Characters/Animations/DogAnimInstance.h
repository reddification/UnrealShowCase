#pragma once

#include "CoreMinimal.h"
#include "AI/Data/DogTypes.h"
#include "Characters/Animations/BaseCharacterAnimInstance.h"
#include "DogAnimInstance.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UDogAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void ApplyState(const UBaseNpcStateSettings* NpcState) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDogAction DogAction;
	
private:
	UPROPERTY()
	const class AAIDogCharacter2* DogCharacter = nullptr;
};
