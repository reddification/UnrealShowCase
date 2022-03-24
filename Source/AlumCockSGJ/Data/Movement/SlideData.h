#pragma once

struct FSlideData
{
	float Speed = 0.f;
	float VerticalSpeed = 0.f;

	float FloorAnglePitch = 0.f;
	float FloorAnglePitchCos = 0.f;
	float FloorAnglePitchSin = 0.f;

	float FloorAngleRoll = 0.f;
	
	FTimerHandle TimerHandle;
	FTimerHandle CooldownTimerHandle;
	bool bCanSlide = true;
};
