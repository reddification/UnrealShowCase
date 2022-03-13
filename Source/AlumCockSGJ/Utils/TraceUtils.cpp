#include "TraceUtils.h"

#include "DrawDebugHelpers.h"

bool TraceUtils::SweepCapsuleSingleByChannel(const UWorld* const World, FHitResult& OutHit, const FVector& Start,
                                               const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, ECollisionChannel TraceChannel,
                                               const FCollisionQueryParams& Params, const FTraceParams& TraceParams, const FQuat& Rot,
                                               const FCollisionResponseParams& ResponseParam)
{
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const bool bHit = World->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CapsuleShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (TraceParams.bDrawDebug)
	{
		DrawDebugCapsule(World, Start, CapsuleHalfHeight, CapsuleRadius,FQuat::Identity, TraceParams.TraceColor,
			false, TraceParams.DrawTime);
		DrawDebugCapsule(World, End, CapsuleHalfHeight, CapsuleRadius,FQuat::Identity, TraceParams.TraceColor,
			false, TraceParams.DrawTime);
		DrawDebugLine(World, Start, End, FColor::Yellow, false,	TraceParams.DrawTime, 0, 2);
		if (bHit)
		{
			DrawDebugCapsule(World, OutHit.Location, CapsuleHalfHeight, CapsuleRadius,
		FQuat::Identity, TraceParams.HitColor, false, TraceParams.DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, TraceParams.HitColor, false,
				TraceParams.DrawTime);	
		}
	}
#endif

	return bHit;
}

bool TraceUtils::SweepSphereSingleByChannel(const UWorld* const World, FHitResult& OutHit, const FVector& Start,
	const FVector& End, float Radius, ECollisionChannel TraceChannel,
	const FCollisionQueryParams& Params, const FTraceParams& TraceParams, const FQuat& Rot,
	const FCollisionResponseParams& ResponseParam)
{
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
	const bool bHit = World->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, SphereShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (TraceParams.bDrawDebug)
	{
		const FVector TraceVector = End-Start;
		float HalfHeight = TraceVector.Size() * 0.5f + Radius;
		const FQuat Quat = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
		// const FQuat Quat = TraceVector.ToOrientationQuat(); // same???
	
		// DrawDebugSphere(World, Start, Radius, 16, TraceParams.TraceColor,false);
		// DrawDebugSphere(World, End, Radius, 16, TraceParams.TraceColor,false);
		// DrawDebugLine(World, Start, End, FColor::Yellow, false,	TraceParams.DrawTime, 0, 2);

		// DrawDebugCapsule(World, (Start + End) * 0.5, HalfHeight, Radius, Quat,
		// 	FColor::Yellow,false, TraceParams.DrawTime);

		DrawDebugCapsule(World, (Start + End) * 0.5, HalfHeight, Radius, Quat, FColor::Yellow);
		
		// if (bHit)
		// {
		// 	DrawDebugSphere(World, OutHit.Location, Radius, 16, TraceParams.HitColor,false, TraceParams.DrawTime);
		// 	DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, FColor::Green, false, TraceParams.DrawTime);
		// }
	}
#endif
	
	return bHit;
}

bool TraceUtils::OverlapCapsuleAnyByProfile(const UWorld* World, const FVector& Location, float Radius, float HalfHeight,
	FName Profile, const FCollisionQueryParams& QueryParams, const FTraceParams& TraceParams, const FQuat& Quat)
{
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
	bool bOverlap = World->OverlapAnyTestByProfile(Location, Quat, Profile, CollisionShape, QueryParams);

#if ENABLE_DRAW_DEBUG
	if (bOverlap && TraceParams.bDrawDebug)
	{
		DrawDebugCapsule(World, Location, HalfHeight, Radius, Quat, FColor::Cyan, false,
			TraceParams.DrawTime, 0, 3);
	}
#endif
	
	return bOverlap;
}

bool TraceUtils::OverlapCapsuleBlockingByProfile(const UWorld* World, const FVector& Location, float Radius, float HalfHeight,
	FName Profile, const FCollisionQueryParams& QueryParams, const FTraceParams& TraceParams, const FQuat& Quat)
{
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
	bool bOverlap = World->OverlapBlockingTestByProfile(Location, Quat, Profile, CollisionShape, QueryParams);

#if ENABLE_DRAW_DEBUG
	if (bOverlap && TraceParams.bDrawDebug)
	{
		DrawDebugCapsule(World, Location, HalfHeight, Radius, Quat, FColor::Cyan, false,
			TraceParams.DrawTime, 0, 3);
	}
#endif
	
	return bOverlap;
}

void TraceUtils::DrawDebugLineTrace(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType,
		bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime, bool isThick)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			::DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime, 0, isThick ? 10 : 0);
			::DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime, 0, isThick ? 10 : 0);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 4, bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}
