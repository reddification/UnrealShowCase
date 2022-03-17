#pragma once

#include "CoreMinimal.h"
#include "AIBaseNpc.h"
#include "AI/Components/Character/NpcLogic/NpcDogLogicComponent.h"
#include "Animations/DogAnimInstance.h"
#include "AIDogCharacter2.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AAIDogCharacter2 : public ABaseCharacter, public IInteractable, public INpcCharacter,
	public IDatatableInitializablePawn
{
	GENERATED_BODY()

public:
	AAIDogCharacter2(const FObjectInitializer& ObjectInitializer);
	EDogAction GetCurrentAction() const { return CurrentAction; }
	void SetCurrentAction(EDogAction NewAction) { CurrentAction = NewAction; }

private:
	EDogAction CurrentAction;

public:
	virtual bool InteractWithPlayer(APlayerCharacter* Interactor) override;
	virtual void SetNpcDTRH(const FDataTableRowHandle& NpcDTRH) override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual UNpcBaseLogicComponent* GetNpcLogicComponent() const override { return NpcLogicComponent; }
	virtual UNpcPlayerInteractionComponent* GetNpcPlayerInteractionComponent() const override { return NpcPlayerInteractionComponent; }

	virtual bool SetDesiredRotation(const FRotator& Rotator) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetNameplateComponent* CharacterInfoWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNpcPlayerInteractionComponent* NpcPlayerInteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNpcDogLogicComponent* NpcLogicComponent;
	
	virtual bool IsPreferStrafing() const override;
	
	virtual UCharacterInteractionOptions* GetInteractionOptions() const override;

	UFUNCTION(BlueprintNativeEvent)
	void InitializeNPC();
	virtual void InitializeNPC_Implementation();
};
