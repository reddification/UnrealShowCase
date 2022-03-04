#pragma once

#include "CoreMinimal.h"
#include "AI/Data/InteractionEqsSettings.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_ActorsOfClass.h"
#include "EnvQueryGenerator_NpcInteractiveActors.generated.h"

UCLASS(meta = (DisplayName = "Interactive actors"))
class ALUMCOCKSGJ_API UEnvQueryGenerator_NpcInteractiveActors : public UEnvQueryGenerator
{
	GENERATED_BODY()

	UEnvQueryGenerator_NpcInteractiveActors();

	/** If true, this will only returns actors of the specified class within the SearchRadius of the SearchCenter context.  If false, it will return ALL actors of the specified class in the world. */
	UPROPERTY(EditDefaultsOnly, Category=Generator)
	FAIDataProviderBoolValue GenerateOnlyActorsInRadius;

	/** Max distance of path between point and context.  NOTE: Zero and negative values will never return any results if
	  * UseRadius is true.  "Within" requires Distance < Radius.  Actors ON the circle (Distance == Radius) are excluded.
	  */
	UPROPERTY(EditDefaultsOnly, Category=Generator)
	FAIDataProviderFloatValue SearchRadius;

	/** context */
	UPROPERTY(EditAnywhere, Category=Generator)
	TSubclassOf<UEnvQueryContext> SearchCenter;

	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;

	bool IsSuitable(AActor* TestActor, const FInteractionEqsSettings& InteractionEqsSettings) const;
};
