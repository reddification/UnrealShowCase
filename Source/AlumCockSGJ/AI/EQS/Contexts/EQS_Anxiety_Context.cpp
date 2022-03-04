#include "EQS_Anxiety_Context.h"

#include "AIController.h"
#include "AI/Interfaces/FeelingsProvider.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UEQS_Anxiety_Context::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                 FEnvQueryContextData& ContextData) const
{
	APawn* QuerierPawn = Cast<APawn>(QueryInstance.Owner.Get());
	if (!IsValid(QuerierPawn))
		return;

	TArray<FVector> ResultingLocations;
	IFeelingsProvider* FeelingsProvider = Cast<IFeelingsProvider>(QuerierPawn->GetController());
	if (FeelingsProvider)
	{
		ResultingLocations = FeelingsProvider->GetAfraidLocations();		
	}
		
	UEnvQueryItemType_Point::SetContextHelper(ContextData, ResultingLocations);
}
