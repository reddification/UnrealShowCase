#pragma once

#include "CoreMinimal.h"
#include "BasePlatform.h"
#include "GameFramework/Actor.h"
#include "LoopedPlatform.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ALoopedPlatform : public ABasePlatform
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Platform)
	float PlatformLoopDelay = 2.0f;

	void ResumePlatform();
	virtual void OnPlatformReachedFinalPosition() override;
	
private:
	FTimerHandle DelayTimer;
};
