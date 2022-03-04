// Copyright 2015 Cameron Angus. All Rights Reserved. /
#pragma once

#include "BTUtilityTypes.h"

struct FBTUtilityMemory;

struct FIndexedUtilityValue
{
	FIndexedUtilityValue() {  }
	FIndexedUtilityValue(int32 ChildIndex, float Utility)
	{
		ChildIdx = ChildIndex;
		UtilityScore = Utility;
	}
	
	int32 ChildIdx;
	float UtilityScore;

	bool operator < (const FIndexedUtilityValue & Other) const
	{
		return UtilityScore > Other.UtilityScore || UtilityScore == Other.UtilityScore && ChildIdx < Other.ChildIdx;
	}
};

namespace UtilitySelection
{
	/**
	* Chooses the node with the highest utility value.
	*/
	void PriorityOrdering(TArray< float > const& UtilityValues, FUtilityExecutionOrdering& ExecutionOrdering);

	void PriorityOrdering2(TArray<float> const& UtilityValues, FBTUtilityMemory* NodeMemory);
}


