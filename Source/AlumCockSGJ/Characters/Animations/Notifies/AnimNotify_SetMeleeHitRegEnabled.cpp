#include "Characters/Animations/Notifies/AnimNotify_SetMeleeHitRegEnabled.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Components/Combat/CharacterCombatComponent.h"

void UAnimNotify_SetMeleeHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	auto Character = GetCharacter(MeshComp->GetOwner());
	if (IsValid(Character))
	{
		Character->GetCombatComponent()->SetMeleeHitRegEnabled(bEnabled);
	}
}
