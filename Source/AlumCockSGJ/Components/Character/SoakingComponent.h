#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "SoakingComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API USoakingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USoakingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	
	void StartSoaking();
	void StopSoaking();
	float GetSoakness() const { return Soakness; }
	bool TryApplySoakness(float SoaknessGain);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const class USoakingSettings* SoakingSettings;
	
private:
	float Soakness = 0.f;
	int PuddlesCount = 0;
	TWeakObjectPtr<UMaterialInstanceDynamic> CardboardMaterial;
	TWeakObjectPtr<ACharacter> CharacterOwner;
	FTimeline SoakingTimeline;
	void Soak(float NewSoakness);
};
