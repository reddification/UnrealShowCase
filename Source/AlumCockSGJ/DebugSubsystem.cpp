#include "DebugSubsystem.h"

bool UDebugSubsystem::IsDebugCategoryEnabled(const FName& CategoryName) const
{
	const bool* bEnabled = CategoriesStates.Find(CategoryName);
	return bEnabled && *bEnabled;
}

void UDebugSubsystem::SetDebugCategoryEnabled(const FName& CategoryName, bool bEnabled)
{
	bool& bCategoryEnabled = CategoriesStates.FindOrAdd(CategoryName);
	bCategoryEnabled = bEnabled;
}

