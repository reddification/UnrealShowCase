#pragma once

#include "CoreMinimal.h"
#include "AI/Components/Character/NpcLogic/NpcHumanoidLogicComponent.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Engine/DataTable.h"
#include "Interfaces/DatatableInitializablePawn.h"
#include "Interfaces/Interactable.h"
#include "AICitizenCharacter.generated.h"

class UNpcBehaviorSettings;

UCLASS()
class ALUMCOCKSGJ_API AAICitizenCharacter : public ABaseHumanoidCharacter, public IInteractable, public INpcCharacter,
	public IDatatableInitializablePawn
{
	GENERATED_BODY()

public:
	AAICitizenCharacter(const FObjectInitializer& ObjectInitializer);

	virtual bool InteractWithPlayer(APlayerCharacter* Interactor) override;
	virtual void SetNpcDTRH(const FDataTableRowHandle& NpcDTRH) override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual UNpcBaseLogicComponent* GetNpcLogicComponent() const override { return NpcLogicComponent; }
	virtual UNpcPlayerInteractionComponent* GetNpcPlayerInteractionComponent() const override { return NpcPlayerInteractionComponent; }
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetNameplateComponent* CharacterInfoWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNpcPlayerInteractionComponent* NpcPlayerInteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNpcConversationComponent* NpcConversationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNpcHumanoidLogicComponent* NpcLogicComponent;
	
	virtual bool IsPreferStrafing() const override;
	
	virtual UCharacterInteractionOptions* GetInteractionOptions() const override;

	UFUNCTION(BlueprintNativeEvent)
	void InitializeNPC();
	virtual void InitializeNPC_Implementation();
};
