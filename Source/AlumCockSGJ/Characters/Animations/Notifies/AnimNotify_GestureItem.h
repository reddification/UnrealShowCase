#pragma once

#include "CoreMinimal.h"
#include "BaseAnimNotify.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AnimNotify_GestureItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UAnimNotify_GestureItem : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GestureItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAttach = true;
};
