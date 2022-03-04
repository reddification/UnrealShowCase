#include "AnimNotify_GestureItem.h"

#include "Subsystems/NpcSubsystem.h"

void UAnimNotify_GestureItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	auto Npc = Cast<INpcCharacter>(MeshComp->GetOwner());
	if (Npc)
		Npc->AttachReactionItem(GestureItemTag, bAttach);
}
