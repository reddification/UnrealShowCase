#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/BaseWorldItem.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/BasicActivityCharacter.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DatatableInitializablePawn.h"
#include "Interfaces/Interactable.h"
#include "AIOutlawCharacter.generated.h"

class ABaseWorldItem;
class UObservationSourceComponent;
// TODO just one class for all NPCs
UCLASS()
class ALUMCOCKSGJ_API AAIOutlawCharacter : public ABaseHumanoidCharacter, public IInteractable, public INpcCharacter,
	public IBasicActivityCharacter, public IDatatableInitializablePawn
{

	GENERATED_BODY()

public:
	AAIOutlawCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	// UCitizenBehaviorSettings* GetBehaviorSettings() const { return AICitizenBehaviorSettings; }

	virtual const FDataTableRowHandle& GetNpcDTRH() const override { return NpcDTRH; }

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	virtual UBehaviorTree* GetDefaultBehaviorTree() const override;

	virtual void SetNpcDTRH(const FDataTableRowHandle& NpcDTRH) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* CharacterInfoWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle NpcDTRH;
	
	virtual void ReactToDamage(
		AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent)
	void InitializeNPC();
	virtual void InitializeNPC_Implementation();

	virtual void SpawnItemsOnDeath() override;
	
private:
	
	TWeakObjectPtr<class UCharacterInfoWidget> CharacterInfoWidget;
	void OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const;
    
	void OnSightStateChanged(bool bInSight);
	int InSightCounter = 0;

	TArray<UObservationSourceComponent*> ObservationSourceComponents;
};
