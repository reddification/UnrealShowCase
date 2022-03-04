#pragma once

#include "PlayerGameStates.generated.h"

UENUM()
enum class EPlayerGameState : uint8
{
	Game,
	Reading,
	Cutscene,
	Dialogue,
	Pause
};