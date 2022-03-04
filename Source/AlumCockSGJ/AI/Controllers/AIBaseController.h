#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/Data/NpcDTR.h"
#include "AIBaseController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTargetKilledEvent, const AActor* Actor);
DECLARE_MULTICAST_DELEGATE(FControllerPawnResetEvent);

UCLASS()
class ALUMCOCKSGJ_API AAIBaseController : public AAIController
{
	GENERATED_BODY()

public:
    AAIBaseController(const FObjectInitializer& ObjectInitializer);
    virtual void SetPawn(APawn* InPawn) override;
    
    void SetInterestingLocation(const FVector& Location);
    void SetDefaultNavigationQueryFilterClass(const TSubclassOf<UNavigationQueryFilter>& OverridingNavigationQueryFilterClass);

    mutable FTargetKilledEvent TargetKilledEvent;
    mutable FControllerPawnResetEvent ControllerPawnResetEvent;

    virtual void OnPossess(APawn* InPawn) override;
    // virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;
    
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UAIBaseActivityComponent* ActivitiesComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UAIBaseReactionComponent* ReactionsComponent;

    static FName ReactionsComponentName;
    static FName ActivitiesComponentName;

    UPROPERTY()
    class ABaseCharacter* ControlledCharacter;
    
    virtual void OnDeath();
};
