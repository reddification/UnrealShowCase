#include "ReadableItem.h"

#include "Characters/BaseCharacter.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Components/QuestGiverComponent.h"
#include "Data/Entities/ReadableDTR.h"

AReadableItem::AReadableItem()
{
	QuestGiverComponent = CreateDefaultSubobject<UQuestGiverComponent>(TEXT("QuestGiver"));
	AddOwnedComponent(QuestGiverComponent);
}

bool AReadableItem::InteractWithPlayer(APlayerCharacter* Interactor)
{
	auto PlayerController = Cast<ABasePlayerController>(Interactor->Controller);
	if (!IsValid(PlayerController))
		return false;
	
	QuestGiverComponent->GiveQuests();
	PlayerController->ReadItem(ItemDTRH);
	return true;
}

void AReadableItem::InitializeWorldItem()
{
	Super::InitializeWorldItem();
	if (!ItemDTRH.IsNull())
	{
		FReadableDTR* ReadableDTR = ItemDTRH.DataTable->FindRow<FReadableDTR>(ItemDTRH.RowName, "");
		if (ReadableDTR)
		{
			StaticMeshComponent->SetStaticMesh(ReadableDTR->StaticMesh);
			QuestGiverComponent->SetQuests(ReadableDTR->BeginQuests);
		}
	}
}