#pragma once

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	Begin,
	InProgress,
	Completed,
	Failed
};

UENUM(BlueprintType)
enum class EContainmentType : uint8
{
	Any,
	All
};

