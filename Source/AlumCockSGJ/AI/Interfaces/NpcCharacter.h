#pragma once

#include "CoreMinimal.h"
#include "AI/Components/Character/NpcPlayerInteractionComponent.h"
#include "AI/Components/Character/NpcLogic/NpcBaseLogicComponent.h"
#include "Components/WidgetNameplateComponent.h"
#include "UObject/Interface.h"
#include "GameplayTags/Public/GameplayTags.h"
#include "NpcCharacter.generated.h"

UINTERFACE()
class UNpcCharacter : public UInterface
{
	GENERATED_BODY()
};

static FDataTableRowHandle CrutchDTRH = FDataTableRowHandle();

class ALUMCOCKSGJ_API INpcCharacter
{
	GENERATED_BODY()

public:
	virtual const FDataTableRowHandle& GetNpcDTRH() const
		{ return GetNpcLogicComponent() ? GetNpcLogicComponent()->GetNpcDTRH() : CrutchDTRH; }
	
	virtual const FGameplayTag& GetState() const
		{ return GetNpcLogicComponent() ?  GetNpcLogicComponent()->GetState() : FGameplayTag::EmptyTag; }
	
	virtual bool TrySetState(const FGameplayTag& GameplayTag)
		{ return GetNpcLogicComponent() ? GetNpcLogicComponent()->TrySetState(GameplayTag) : false; }
	
	virtual bool StartReaction(const FGameplayTag& GameplayTag, int ReactionIndex)
		{ return GetNpcLogicComponent() ? GetNpcLogicComponent()->StartReaction(GameplayTag, ReactionIndex) : false; }
	
	virtual bool StopReaction(const FGameplayTag& GameplayTag)
		{ return GetNpcLogicComponent() ? GetNpcLogicComponent()->StopReaction(GameplayTag) : false; }
	
	virtual FGameplayTag GetRunningReaction() const
		{ return GetNpcLogicComponent() ? GetNpcLogicComponent()->GetRunningReaction() : FGameplayTag::EmptyTag; }
	
	virtual void SetDesiredSpeed(float DesiredSpeed) { if (GetNpcLogicComponent()) GetNpcLogicComponent()->SetDesiredSpeed(DesiredSpeed); }
	virtual void ResetDesiredSpeed() { SetDesiredSpeed(-1.f); } 
	
	virtual bool IsAiForcingStrafing() const { return GetNpcLogicComponent() ? GetNpcLogicComponent()->IsAiForcingStrafing() : false; }
	virtual void SetAiForcedStrafing(bool bNewState) { if (GetNpcLogicComponent()) GetNpcLogicComponent()->SetAiForcedStrafing(bNewState); }
	
	virtual bool IsAiEnabled() const { return GetNpcLogicComponent() ? GetNpcLogicComponent()->IsAiEnabled() : false; }
	virtual void AttachReactionItem(const FGameplayTag& ItemTag, bool bAttach) { if (GetNpcLogicComponent()) GetNpcLogicComponent()->AttachReactionItem(ItemTag, bAttach); }

	virtual bool SetAiDesiredRotation(const FRotator& DesiredRotation) const
		{ return GetNpcLogicComponent() ? GetNpcLogicComponent()->SetDesiredRotation(DesiredRotation) : false; }


	virtual void OnInteractionFinished() { if (GetNpcPlayerInteractionComponent()) GetNpcPlayerInteractionComponent()->ResetInteractionState(); } 
	
	virtual UNpcPlayerInteractionComponent* GetNpcPlayerInteractionComponent() const { return nullptr; }
	virtual UNpcBaseLogicComponent* GetNpcLogicComponent() const { return nullptr; }
};
