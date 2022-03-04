#pragma once

#include "CoreMinimal.h"
#include "ObservationSourceComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSightStateChangedEvent, bool bInSight);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UObservationSourceComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	FSightStateChangedEvent SightStateChangedEvent;
	
	void UpdateVisibility();
	bool IsSightSourceActive() const { return bSightSourceActive; }
	void SetDisabled() { bSightSourceActive = false; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UObservationSourceSettings* ObservationSourceSettings;
	
private:
	float ObservationSquaredDistanceThreshold = 0.f;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	
	bool bInSight = false;
	bool bSightSourceActive = false;
};
