#include "BTDecorator_Maybe.h"

UBTDecorator_Maybe::UBTDecorator_Maybe()
{
	NodeName = "Maybe";
}

bool UBTDecorator_Maybe::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return FMath::RandRange(0.f, 1.f) <= Probability;
}

FString UBTDecorator_Maybe::GetStaticDescription() const
{
	return FString::Printf(TEXT("Probability: %.2f"), Probability);
}
