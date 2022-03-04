#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "QuestGiverComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UQuestGiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void GiveQuests();
	void SetQuests(TArray<FDataTableRowHandle>& Quests);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="QuestDTR"))
	TArray<FDataTableRowHandle> QuestsDTRHs;
};
