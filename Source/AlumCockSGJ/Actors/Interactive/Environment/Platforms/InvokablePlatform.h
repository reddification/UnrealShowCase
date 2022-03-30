#pragma once

#include "CoreMinimal.h"
#include "BasePlatform.h"
#include "InvokablePlatform.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AInvokablePlatform : public ABasePlatform
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPlatformReachedFinalPosition() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	class APlatformInvocator* PlatformInvocator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bWaitUntilInvokerReleased = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bWaitWhileOverlapping == false"))
	float ReleasePlatformAfter = 3.f;
	
private:
	FTimerHandle PlatformReturnDelayTimer;
	void ReturnPlatform();
	
	void OnPlatformInvoked(bool bInvoked);
};
