#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_HasGameplayTags.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UEnvQueryTest_HasGameplayTags : public UEnvQueryTest
{
	GENERATED_BODY()
	
public:
	UEnvQueryTest_HasGameplayTags(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * @note Calling function only makes sense before first run of given query
	 * by the EQS manager. The query gets preprocessed and cached then so the query 
	 * value will get stored and calling this function will not change it (unless 
	 * you call it on the cached test's instance, see UEnvQueryManager::CreateQueryInstance).
	 */
	void SetTagQueryToMatch(FGameplayTagQuery& GameplayTagQuery);

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionDetails() const override;

	bool SatisfiesTest(IGameplayTagAssetInterface* ItemGameplayTagAssetInterface) const;

	UPROPERTY(EditAnywhere, Category=GameplayTagCheck)
	FGameplayTagQuery TagQueryToMatch;
};
