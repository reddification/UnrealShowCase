#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "CommonConstants.h"
#include "AI/Data/AITypesGC.h"
#include "Interfaces/KillConsiderate.h"
#include "AIOutlawController.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AAIOutlawController : public AAIBaseController, public IKillConsiderate
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIOutlawController(const FObjectInitializer& ObjectInitializer);

	virtual void OnTargetKilled(const AActor* Actor);
	void ResetFocusedCharacter();
	virtual void ReportKill(const AActor* KilledActor) override;
	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority) override;

	virtual void GetPlayerViewPoint(FVector& Location, FRotator& Rotation) const override;
	
protected:
	virtual void BeginPlay() override;

	// perhaps move it to bot character class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DefaultAimSocket = AISocketDefaultCharacterTarget;

	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;
	
private:
	EAICombatReason CombatReason = EAICombatReason::None;

	TWeakObjectPtr<ABaseCharacter> ControlledCharacter;
	TWeakObjectPtr<ABaseCharacter> FocusedCharacter;
};
