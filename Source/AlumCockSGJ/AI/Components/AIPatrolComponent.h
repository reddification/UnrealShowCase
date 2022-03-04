#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/PatrolSettings.h"
#include "AIPatrolComponent.generated.h"

class APatrolPath;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALUMCOCKSGJ_API UAIPatrolComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	FVector SelectClosestWaypoint();
	FVector SelectNextWaypoint();
	bool CanPatrol() const;
	bool IsAggressive() const { return CanPatrol() && PatrolSettings.PatrolMode == EPatrolMode::Aggressive; }
	FVector GetFocusPoint() const;
	AActor* GetFocusActor() const;

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FPatrolSettings PatrolSettings;

private:
	int32 SelectedWaypointIndex = -1;
};
