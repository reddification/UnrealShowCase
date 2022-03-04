#include "AnimNotify_CarryingStateChanged.h"

#include "Characters/BaseHumanoidCharacter.h"

void UAnimNotify_CarryingStateChanged::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	auto Character = GetCharacter(MeshComp->GetOwner());
	if (Character)
	{
		if (bCarryingNow)
			Character->GetCarryingComponent()->AttachItemToCharacter();
		else 
			Character->GetCarryingComponent()->DetachItemFromCharacter();
	}
}
