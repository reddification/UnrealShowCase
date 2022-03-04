#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Interfaces/GameplayTagActor.h"
#include "Interfaces/Interactable.h"
#include "BaseWorldItem.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class ALUMCOCKSGJ_API ABaseWorldItem : public AActor, public IInteractable, public IGameplayTagActor
{
	GENERATED_BODY()

public:
	ABaseWorldItem();

	const FDataTableRowHandle& GetItemDTR() const { return ItemDTRH; }
	void SetItemDTRH(const FDataTableRowHandle& NewItemDTRH);

	virtual void InSight() override;
	virtual void OutOfSight() override;

	virtual void ChangeGameplayTags(const FGameplayTagContainer& TagsContainer, bool bAdd) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle ItemDTRH;

	/* UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* BillboardComponent;
      */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* ItemInfoWidgetComponent;
	
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void InitializeWorldItem();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer GameplayTags;
};