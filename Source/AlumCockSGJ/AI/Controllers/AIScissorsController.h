#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "CommonConstants.h"
#include "AI/Data/AITypesGC.h"
#include "Data/PerceptionData.h"
#include "Interfaces/KillConsiderate.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIScissorsController.generated.h"

class ABaseCharacter;
class ABaseHumanoidCharacter;

UCLASS()
class ALUMCOCKSGJ_API AAIScissorsController : public AAIBaseController, public IKillConsiderate
{
	GENERATED_BODY()

public:
    AAIScissorsController(const FObjectInitializer& ObjectInitializer);
    
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