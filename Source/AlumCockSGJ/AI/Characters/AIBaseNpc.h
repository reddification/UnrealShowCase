#pragma once

#include "CoreMinimal.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/DatatableInitializablePawn.h"
#include "Interfaces/Interactable.h"
#include "AIBaseNpc.generated.h"

class UObservationSourceComponent;
class UNpcHumanoidStateSettings;
// TODO copypaste of AICitizen. Refactor to have AIBaseNpc as a parent for dog and citizen
UCLASS()
class ALUMCOCKSGJ_API AAIBaseNpc : public ABaseCharacter, public IInteractable, public INpcCharacter,
	public IDatatableInitializablePawn
{
	GENERATED_BODY()

public:
	AAIBaseNpc(const FObjectInitializer& ObjectInitializer);

	virtual const FDataTableRowHandle& GetNpcDTRH() const override { return NpcDTRH; }

	virtual const FGameplayTag& GetState() const override;
	virtual bool TrySetState(const FGameplayTag& GameplayTag) override;
	
	virtual void ApplyState(const UBaseNpcStateSettings* NpcState);

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	virtual void OnOutOfHealth() override;

	virtual bool StartReaction(const FGameplayTag& GameplayTag, int ReactionIndex) override;
	virtual bool StopReaction(const FGameplayTag& GameplayTag) override;
	virtual bool IsAiEnabled() const override;

	virtual bool InteractWithPlayer(APlayerCharacter* Interactor) override;
	virtual void SetNpcDTRH(const FDataTableRowHandle& NpcDTRH) override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual void SetAiForcedStrafing(bool bNewState) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetNameplateComponent* CharacterInfoWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNpcPlayerInteractionComponent* NpcPlayerInteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle NpcDTRH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAiEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery IgnoreAiForcedStrafeInStates;
	
	virtual void ReactToDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

	virtual bool IsPreferStrafing() const override;
	
	virtual UCharacterInteractionOptions* GetInteractionOptions() const override;

	UFUNCTION(BlueprintNativeEvent)
	void InitializeNPC();
	virtual void InitializeNPC_Implementation();
	
private:

	bool bPreferStrafingState = false;
	bool bAiForceStrafing = false;

	TWeakObjectPtr<class UCharacterInfoWidget> CharacterInfoWidget;
	void OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const;
    
	void OnSightStateChanged(bool bInSight);
	int InSightCounter = 0;

	TArray<UObservationSourceComponent*> ObservationSourceComponents;

	FGameplayTag CharacterStateTag;
	FGameplayTag ActiveReactionTag;
	int ActiveReactionIndex = -1;
	
	void RegisterNpcSubsystem();
};
