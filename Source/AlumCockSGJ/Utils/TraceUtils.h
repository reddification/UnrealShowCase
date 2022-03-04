#pragma once
#include "Kismet/KismetSystemLibrary.h"

namespace TraceUtils
{
	struct FTraceParams
	{
		FTraceParams()
		{
		}

		FTraceParams(bool bDrawDebug)
		{
			this->bDrawDebug = bDrawDebug;
		}
		
		bool bDrawDebug = false;
		float DrawTime = 2;
		FColor TraceColor = FColor::Red;
		FColor HitColor = FColor::Green;
	};
	
	bool SweepCapsuleSingleByChannel(const class UWorld* const World, struct FHitResult& OutHit,
		const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight,
		ECollisionChannel TraceChannel,
		const FCollisionQueryParams& Params,
		const FTraceParams& TraceParams,
		const FQuat& Rot = FQuat::Identity,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);

	bool SweepSphereSingleByChannel(const class UWorld* const World, struct FHitResult& OutHit,
		const FVector& Start, const FVector& End, float Radius, ECollisionChannel TraceChannel,
		const FCollisionQueryParams& Params,
		const FTraceParams& TraceParams, const FQuat& Rot = FQuat::Identity,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);

	bool OverlapCapsuleAnyByProfile(const class UWorld* World, const FVector& Location,
		float Radius, float HalfHeight, FName Profile, 
		const FCollisionQueryParams& QueryParams, const FTraceParams& TraceParams,
		const FQuat& Quat = FQuat::Identity);

	bool OverlapCapsuleBlockingByProfile(const class UWorld* World, const FVector& Location,
		float Radius, float HalfHeight, FName Profile, 
		const FCollisionQueryParams& QueryParams, const FTraceParams& TraceParams,
		const FQuat& Quat = FQuat::Identity);
	
	void DrawDebugLineTrace(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType,
		bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime, bool isThick);
}
