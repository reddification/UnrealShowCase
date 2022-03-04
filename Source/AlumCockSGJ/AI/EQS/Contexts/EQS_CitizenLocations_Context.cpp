#include "EQS_CitizenLocations_Context.h"

#include "AI/EQS/EQS_TagTarget.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"

void UEQS_CitizenLocations_Context::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                   FEnvQueryContextData& ContextData) const
{
	if (!QueryInstance.Owner.IsValid())
		return;

	TArray<AActor*> TagTargets;
	UGameplayStatics::GetAllActorsOfClass(QueryInstance.World,  AEQS_TagTarget::StaticClass(), TagTargets);
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, TagTargets);
}
