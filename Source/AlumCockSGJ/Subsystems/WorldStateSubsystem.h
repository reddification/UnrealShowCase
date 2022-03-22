#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WorldStateSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FWorldStateChangedEvent, const FGameplayTagContainer& NewGameState);

UCLASS()
class ALUMCOCKSGJ_API UWorldStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void ChangeWorldState(const FGameplayTagContainer& TagsContainer, bool bAdd = true);
	bool IsWorldStateMatches(const FGameplayTagQuery& WorldStateTagQuery) const;
	const FGameplayTagContainer& GetWorldState() const { return WorldState; }
	void Load();

	mutable FWorldStateChangedEvent WorldStateChangedEvent;
	
private:
	FGameplayTagContainer WorldState;
};
