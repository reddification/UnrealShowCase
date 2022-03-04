#include "BTDecorator_TimeLimitDeviating.h"

void UBTDecorator_TimeLimitDeviating::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	TimeLimit = FMath::FRandRange(FMath::Max(0.0f, TimeLimit - RandomDeviation), (TimeLimit + RandomDeviation));
	Super::OnNodeActivation(SearchData);
}

FString UBTDecorator_TimeLimitDeviating::GetStaticDescription() const
{
	return FString::Printf(TEXT("Abort after %.2fs +- %.2fs"), TimeLimit, RandomDeviation);
}
