// Copyright 2015 Cameron Angus. All Rights Reserved.

#include "UtilitySelectionMethods/BTUtilitySelectionMethod_Highest.h"

#include "Composites/BTComposite_Utility.h"

namespace UtilitySelection
{
	TArray<FIndexedUtilityValue> GetIndexedValues(TArray<float> const& UtilityValues, int Count)
	{
		TArray<FIndexedUtilityValue> IndexedValues;
		IndexedValues.SetNumUninitialized(Count);
		for (int32 ChildIdx = 0; ChildIdx < Count; ++ChildIdx)
		{
			IndexedValues[ChildIdx].ChildIdx = ChildIdx;
			IndexedValues[ChildIdx].UtilityScore = UtilityValues[ChildIdx];
		}

		IndexedValues.Sort();
		return IndexedValues;
	}
	
	void PriorityOrdering(TArray<float> const& UtilityValues, FUtilityExecutionOrdering& ExecutionOrdering)
	{
		// Index the values and sort them
	
		auto const Count = UtilityValues.Num();
		const auto& IndexedValues = GetIndexedValues(UtilityValues, Count); 
		// Output the sorted indexes
		ExecutionOrdering.SetNumUninitialized(Count);
		for (int32 ExIdx = 0; ExIdx < Count; ++ExIdx)
		{
			ExecutionOrdering[ExIdx] = IndexedValues[ExIdx].ChildIdx;
		}
	}

	void PriorityOrdering2(TArray<float> const& UtilityValues, FBTUtilityMemory* NodeMemory)
	{
		// Index the values and sort them
		auto const Count = UtilityValues.Num();
		NodeMemory->ExecutionUtilityOrdering = GetIndexedValues(UtilityValues, Count); 
		
		// // Output the sorted indexes
		// NodeMemory->ExecutionOrdering.SetNumUninitialized(Count);
		// NodeMemory->UtilityScores.SetNumUninitialized(Count);
		// for (int32 ExIdx = 0; ExIdx < Count; ++ExIdx)
		// {
		// 	NodeMemory->ExecutionOrdering[ExIdx] = IndexedValues[ExIdx].ChildIdx;
		// 	NodeMemory->UtilityScores[ExIdx] = IndexedValues[ExIdx].UtilityScore;
		// 	NodeMemory->ExecutionUtilityOrdering.Add(FIndexedUtilityValue())
		// }
	}
}


#if 0

int32 UBTUtilitySelectionMethod_Highest::SelectOption(TArray< float > const& UtilityValues)
{
	int32 HighestIdx = NoSelection;
	float HighestVal = 0.0f;
	
	for (int32 Idx = 0; Idx < UtilityValues.Num(); ++Idx)
	{
		if (UtilityValues[Idx] > HighestVal)
		{
			HighestIdx = Idx;
			HighestVal = UtilityValues[Idx];
		}
	}

	return HighestIdx;
}

void UBTUtilitySelectionMethod_Highest::GenerateOrdering(TArray< float > const& UtilityValues, FUtilityExecutionOrdering& ExecutionOrdering)
{
	struct FIndexedUtilityValue
	{
		int32 ChildIdx;
		float Value;

		inline bool operator< (FIndexedUtilityValue const& Other) const
		{
			return Value > Other.Value;
		}
	};

	// Index the values and sort them
	auto const Count = UtilityValues.Num();
	TArray< FIndexedUtilityValue > IndexedValues;
	IndexedValues.SetNumUninitialized(Count);
	for (int32 ChildIdx = 0; ChildIdx < Count; ++ChildIdx)
	{
		IndexedValues[ChildIdx].ChildIdx = ChildIdx;
		IndexedValues[ChildIdx].Value = UtilityValues[ChildIdx];
	}

	IndexedValues.Sort();

	// Output the sorted indexes
	ExecutionOrdering.SetNumUninitialized(Count);
	for (int32 ExIdx = 0; ExIdx < Count; ++ExIdx)
	{
		ExecutionOrdering[ExIdx] = IndexedValues[ExIdx].ChildIdx;
	}
}

FString UBTUtilitySelectionMethod_Highest::GetDesc()
{
	return FString::Printf(TEXT("Highest Utility"));
}

#endif

