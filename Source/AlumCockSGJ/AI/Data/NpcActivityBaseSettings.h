#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/NpcInteraction/NpcBaseInteractiveActor.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "NpcActivityBaseSettings.generated.h"

class ANpcBaseInteractiveActor;

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityBaseSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="QuestLocationDTR"))
	FDataTableRowHandle BaseActivityLocationDTRH;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, UEnvQuery*> EnvQueries;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UNavigationQueryFilter> ActivityNavigationFilterClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TODO|Progress", meta=(UIMin = -1.f, UIMax = 1.f, ClampMin = -1.f, ClampMax = 1.f))
	float EnergyChange = -0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TODO|Progress",  meta=(UIMin = 0.001f, ClampMin = 0.001f))
	float EnergyChangeRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TODO|Progress", meta=(UIMin=0.1f, ClampMin = 0.1f))
	float BoredomChange = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TODO|Progress", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float BoredomChangeRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TODO|Progress", meta=(UIMin=0.1f, ClampMin = 0.1f))
	float UtilityChange = -0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TODO|Progress", meta=(UIMin=0.f, UIMax=1.f, ClampMin = 0.f, ClampMax = 1.f))
	float UtilityChangeRate = 1.f;

};

USTRUCT(BlueprintType)
struct FWanderAroundLocationDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="QuestLocationDTR"))
	FDataTableRowHandle LocationDTRH;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float StayTime;
};

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityWanderAroundSettings : public UNpcActivityBaseSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWanderAroundLocationDescriptor> LocationsDescriptors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRandomPick;
};

USTRUCT(BlueprintType)
struct FRestActivityInteractionActorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery ActorTagQuery;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MustImplement=NpcInteractableActor))
	TArray<TSubclassOf<AActor>> ActorsToInteractWith;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InteractionDurationTimeMin = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InteractionDurationTimeMax = 60.f;
};

UCLASS()
class UNpcActivityStayInLocationSettings : public UNpcActivityBaseSettings
{
	GENERATED_BODY()
};
