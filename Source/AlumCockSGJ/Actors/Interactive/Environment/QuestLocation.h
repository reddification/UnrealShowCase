#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "SaveSystem/SaveSubsystemInterface.h"
#include "QuestLocation.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AQuestLocation : public AActor, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:
	AQuestLocation();
	
	const FDataTableRowHandle& GetLocationDTRH() const { return QuestLocationDTRH; }

	FVector GetRandomLocationInVolume(float FloorOffset) const;
	TArray<AActor*> GetOverlappedActorsInVolume(const TSubclassOf<AActor>& ActorTypeOfInterest) const;

	virtual void BeginDestroy() override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, meta=(RowType="QuestLocationDTR"))
	FDataTableRowHandle QuestLocationDTRH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bQuestLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag LocationGameplayTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UQuestGiverComponent* QuestGiverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* CollisionComponent;

private:
	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	void OnOverlappedActor(AActor* OtherActor);
    virtual void OnLevelDeserialized_Implementation() override;

	FTimerHandle InitialOverlapTimer;
	void CheckOverlappingActors();
};