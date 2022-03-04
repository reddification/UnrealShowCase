#pragma once

#include "CoreMinimal.h"
#include "Characters/Animations/Notifies/BaseAnimNotify.h"
#include "AnimNotify_SetMeleeHitRegEnabled.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UAnimNotify_SetMeleeHitRegEnabled : public UBaseAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};
