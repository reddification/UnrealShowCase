#pragma once

#include "CoreMinimal.h"
#include "BaseAnimNotify.h"
#include "AnimNotify_CarryingStateChanged.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UAnimNotify_CarryingStateChanged : public UBaseAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCarryingNow = false;
};
