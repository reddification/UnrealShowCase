#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AI/Data/AITypesGC.h"
#include "AI/Data/NpcHumanoidStateSettings.h"
#include "Characters/BaseCharacter.h"
#include "Components/ActorComponent.h"
#include "Data/CharacterTypes.h"
#include "Engine/DataTable.h"
#include "NpcBaseLogicComponent.generated.h"

class UCharacterInteractionOptions;
class UObservationSourceComponent;
class ABaseCharacter;

DECLARE_MULTICAST_DELEGATE_TwoParams(FNpcDiedEvent, const FDataTableRowHandle& NpcDTRH, ABaseCharacter* NpcCharacter);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FNpcReactionStateChangedEvent, ABaseCharacter* Npc, const FGameplayTag& StartedReactionTag, bool bActive);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UNpcBaseLogicComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual const FDataTableRowHandle& GetNpcDTRH() const { return NpcDTRH; }

	virtual const FGameplayTag& GetState() const;
	virtual bool TrySetState(const FGameplayTag& GameplayTag);
	
	virtual void OnDeath(const ABaseCharacter* Character);

	virtual bool StartReaction(const FGameplayTag& GameplayTag, int ReactionIndex);
	virtual bool StopReaction(const FGameplayTag& GameplayTag);
	virtual void AttachReactionItem(const FGameplayTag& ItemTag, bool bAttach);
	
	virtual bool IsAiEnabled() const;

	virtual void SetNpcDTRH(const FDataTableRowHandle& NpcDTRH);

	virtual void GetNpcLogicTags(FGameplayTagContainer& TagContainer) const;
	
    virtual void SetAiForcedStrafing(bool bNewState);
	FGameplayTag GetRunningReaction() const { return ActiveReactionTag; }
	bool IsAiForcingStrafing() const { return bAiForceStrafing; }
	void SetDesiredSpeed(float DesiredSpeed);
	bool SetDesiredRotation(const FRotator& Rotation) const;

	mutable FNpcDiedEvent NpcDiedEvent;
	mutable FNpcReactionStateChangedEvent NpcReactionStateChangedEvent;

	virtual bool IsPreferStrafing() const;

	void InitializeNPC();
	virtual UCharacterInteractionOptions* GetInteractionOptions() const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle NpcDTRH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAiEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery IgnoreAiForcedStrafeInStates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSpawnItemDescription> SpawnItemsAfterDeath;
	
	UFUNCTION()
	virtual void ReactToDamage(
		AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	virtual void ApplyState(const UBaseNpcStateSettings* NpcState);

	UPROPERTY()
	ABaseCharacter* OwnerCharacter;
	
private:

	bool bPreferStrafingState = false;
	bool bAiForceStrafing = false;

	TWeakObjectPtr<class UCharacterInfoWidget> CharacterInfoWidget;
	void OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const;
    
	void OnSightStateChanged(bool bInSight);
	int InSightCounter = 0;

	UPROPERTY()
	TArray<UObservationSourceComponent*> ObservationSourceComponents;

	UPROPERTY()
	class UWidgetNameplateComponent* CharacterInfoWidgetComponent = nullptr;
	
	FGameplayTag CharacterStateTag;
	FGameplayTag ActiveReactionTag;
	int ActiveReactionIndex = -1;
	
	UPROPERTY()
	TMap<FGameplayTag, UStaticMeshComponent*> ReactionAttachedItems;
	
	void RegisterNpcSubsystem() const;
};
