#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:
	const TArray<FVector>& GetWaypoints() const { return Waypoints; }
	void Reverse();
	FVector GetFocusPoint() const;
	FVector GetGuardLocation() const;
	AActor* GetFocusActor () const { return ViewActor; }

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(MakeEditWidget))
	TArray<FVector> Waypoints;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(MakeEditWidget))
	FVector ViewCenter;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(MakeEditWidget))
	FVector GuardLocation;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	AActor* ViewActor;
};
