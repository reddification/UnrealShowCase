#pragma once

#include "CoreMinimal.h"
#include "Puddle.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APuddle : public AActor
{
	GENERATED_BODY()

public:
	APuddle();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	class UStaticMeshComponent* PuddleMeshComponent;

private:
	UFUNCTION()
	void OnPuddleSteppedInto(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPuddleSteppedOut(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
