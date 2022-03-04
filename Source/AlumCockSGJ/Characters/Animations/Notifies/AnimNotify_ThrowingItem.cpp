#include "Characters/Animations/Notifies/AnimNotify_ThrowingItem.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Combat/CharacterCombatComponent.h"

void UAnimNotify_ThrowingItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	auto Character = GetCharacter(MeshComp->GetOwner());
	if (!Character) return;

	switch (ThrowNotifyType)
	{
		case EThrowNotifyType::Grab:
			Character->GetCombatComponent()->GrabThrowableItem();
			break;
		case EThrowNotifyType::Activate:
			Character->GetCombatComponent()->ActivateThrowableItem();
			break;
		case EThrowNotifyType::Release:
			Character->GetCombatComponent()->ReleaseThrowableItem();
			break;
		default:
			break;
	}
}
