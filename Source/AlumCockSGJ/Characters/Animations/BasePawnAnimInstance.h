#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BasePawnAnimInstance.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Base pawn animation instance")
	float InputForward = 0;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Base pawn animation instance")
	float InputRight = 0;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Base pawn animation instance")
	bool bInAir = false;

private:
	TWeakObjectPtr<class ABasePawn> Pawn;
};
