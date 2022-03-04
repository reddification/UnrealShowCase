#pragma once

#include "DebugEnums.generated.h"

UENUM(BlueprintType)
enum class EDebugMode : uint8 {
	None = 0 UMETA(DisplayName = "None"),
	Brief = 1 UMETA(DisplayName = "Brief"),
	Verbose = 2 UMETA(DisplayName = "Verbose"),
	UltraVerbose = 3 UMETA(DisplayName = "UltraVerbose"),
};

UENUM(BlueprintType)
enum class EDebugTraceUnderCursorMode : uint8
{
    None = 0 UMETA(DisplayName = "None"),
    Visible = 1 UMETA(DisplayName = "Visible")
};