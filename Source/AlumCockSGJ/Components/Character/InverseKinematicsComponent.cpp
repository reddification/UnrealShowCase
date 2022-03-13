#include "InverseKinematicsComponent.h"

#include "CommonConstants.h"
#include "DebugSubsystem.h"
#include "Data/DataAssets/Components/Movement/IKSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// TODO move to parameters
const float FootOffset = 15.f;
const FName LeftFootBoneName = "foot_l";
const FName RightFootBoneName = "foot_r";

void UInverseKinematicsComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!IkSettings)
	{
		UE_LOG(LogTemp, Log, TEXT("IKComponent settings not set. IK won't work for %s"), *GetOwner()->GetName());
		bInitialized = false;
	}
	else
	{
		bInitialized = true;
	}
}

void UInverseKinematicsComponent::CalculateIkData(const USkeletalMeshComponent* SkeletalMesh, float CapsuleHalfHeight,
	FVector ActorLocation, bool bCrouched, float DeltaTime)
{
	if (!bInitialized)
		return;
	
	UpdateLegsIkOffsetsBoxTrace(SkeletalMesh, CapsuleHalfHeight, ActorLocation, bCrouched, DeltaTime);
	RecalculateFeetPitches(SkeletalMesh, DeltaTime);
	RecalculateKneesExtends(DeltaTime);
}

void UInverseKinematicsComponent::UpdateLegsIkOffsetsBoxTrace(const USkeletalMeshComponent* SkeletalMesh, float CapsuleHalfHeight,
                                                              const FVector& ActorLocation, bool bCrouched, float DeltaSeconds)
{
	const float PreviousRightFootElevation = IkData.RightFootElevation;
	const float PreviousLeftFootElevation = IkData.LeftFootElevation;
	const float PreviousPelvisElevation = IkData.PelvisElevation;
	
	IkData.RightFootElevation = FMath::RoundToFloat(GetIkElevationForSocket(IkSettings->RightFootSocketName, RightFootBoneName,
		SkeletalMesh, ActorLocation, CapsuleHalfHeight, bCrouched));
	IkData.LeftFootElevation = FMath::RoundToFloat(GetIkElevationForSocket(IkSettings->LeftFootSocketName, LeftFootBoneName,
		SkeletalMesh, ActorLocation, CapsuleHalfHeight, bCrouched));
	
	RecalculateFeetElevationsWithPelvis();

	if (IkData.RightFootElevation != PreviousRightFootElevation)
	{
		IkData.RightFootElevation = FMath::FInterpTo(PreviousRightFootElevation, IkData.RightFootElevation,
		DeltaSeconds, IkSettings->LegElevationInterpSpeed);
	}
	if (IkData.LeftFootElevation != PreviousLeftFootElevation)
	{
		IkData.LeftFootElevation = FMath::FInterpTo(PreviousLeftFootElevation, IkData.LeftFootElevation,
			DeltaSeconds, IkSettings->LegElevationInterpSpeed);
	}
	if (IkData.PelvisElevation != PreviousPelvisElevation)
	{
		IkData.PelvisElevation = FMath::FInterpTo(PreviousPelvisElevation, IkData.PelvisElevation,
		DeltaSeconds, IkSettings->LegElevationInterpSpeed);
	}
}

void UInverseKinematicsComponent::RecalculateFeetElevationsWithPelvis()
{
	// should be close to zero at the end of the day
	const float ElevationThreshold = -1;
	if (IkData.LeftFootElevation < ElevationThreshold)
	{
		IkData.PelvisElevation = IkData.LeftFootElevation;
		IkData.RightFootElevation += -IkData.LeftFootElevation;
		IkData.LeftFootElevation = 0;
	}
	else if (IkData.RightFootElevation < ElevationThreshold)
	{
		IkData.PelvisElevation = IkData.RightFootElevation;
		IkData.LeftFootElevation += -IkData.RightFootElevation;
		IkData.RightFootElevation = 0;
	}
	else
	{
		IkData.PelvisElevation = 0;
	}
}

// TODO just use surface normal you idiot
void UInverseKinematicsComponent::RecalculateFeetPitches(const USkeletalMeshComponent* SkeletalMesh, float DeltaSeconds)
{
	const float PreviousRightFootPitch = IkData.RightFootPitch;
	const float PreviousLeftFootPitch = IkData.LeftFootPitch;
	
	const float NewRightFootPitch = FMath::RoundToFloat(CalculateFootPitch(SkeletalMesh, IkSettings->RightHeelSocketName,
	IkSettings->RightFootSocketName, IkSettings->RightToesSocketName, RightFootBoneName, PreviousRightFootPitch));
	const float NewLeftFootPitch = FMath::RoundToFloat(CalculateFootPitch(SkeletalMesh, IkSettings->LeftHeelSocketName,
	IkSettings->LeftFootSocketName, IkSettings->LeftToesSocketName, LeftFootBoneName, PreviousLeftFootPitch));

	UE_LOG(LogTemp, Log, TEXT("PRFP: %.2f, NRFP: %.2f"), PreviousRightFootPitch, NewRightFootPitch);
	UE_LOG(LogTemp, Log, TEXT("PLFP: %.2f, NLFP: %.2f"), PreviousLeftFootPitch, NewLeftFootPitch);

	const float ChangePitchThreshold = 2.f;
	if (!FMath::IsNearlyEqual(PreviousRightFootPitch, NewRightFootPitch))
	{
		IkData.RightFootPitch = FMath::FInterpTo(PreviousRightFootPitch, NewRightFootPitch,
		DeltaSeconds, IkSettings->FeetPivotInterpSpeed);
	}

	if (!FMath::IsNearlyEqual(PreviousLeftFootPitch, NewLeftFootPitch))
	{
		IkData.LeftFootPitch = FMath::FInterpTo(PreviousLeftFootPitch, NewLeftFootPitch,
		DeltaSeconds, IkSettings->FeetPivotInterpSpeed);
	}
}

void UInverseKinematicsComponent::RecalculateKneesExtends(float DeltaSeconds)
{
	const float PreviousRightKneeOutwardExtend = IkData.RightKneeOutwardExtend;
	const float PreviousLeftKneeOutwardExtend = IkData.LeftKneeOutwardExtend;
	IkData.RightKneeOutwardExtend = FMath::FInterpTo(PreviousRightKneeOutwardExtend,
		FMath::Lerp(0.f, IkSettings->MaxKneeOutwardExtend, IkData.RightFootElevation/IkSettings->MaxKneeOutwardExtend), DeltaSeconds,
		IkSettings->LegElevationInterpSpeed);
	IkData.LeftKneeOutwardExtend = FMath::FInterpTo(PreviousLeftKneeOutwardExtend,
		FMath::Lerp(0.f, IkSettings->MaxKneeOutwardExtend, IkData.LeftFootElevation/IkSettings->MaxKneeOutwardExtend), DeltaSeconds,
		IkSettings->LegElevationInterpSpeed);
}

float UInverseKinematicsComponent::GetIkElevationForSocket(const FName& SocketName, const FName& FootBoneName, const USkeletalMeshComponent* SkeletalMesh,
	const FVector& ActorLocation, float CapsuleHalfHeight, bool bCrouched)
{
	const float TraceDistance = IkData.IKScale * (bCrouched
		? IkSettings->TraceElevationCrouched
		: IkSettings->TraceElevation);

	FTransform FootSocketTransform = SkeletalMesh->GetSocketTransform(SocketName);
	FVector FootBoneLocation = SkeletalMesh->GetBoneLocation(FootBoneName);
	FVector FootSocketLocation = FootSocketTransform.GetLocation();
	FootSocketLocation.Z = FootBoneLocation.Z - FootOffset;
	FRotator FootRotation = FootSocketTransform.Rotator();
	if (FootRotation.Pitch < IkSettings->MinFootPitchForElevationTrace && !bCrouched)
	{
		return 0;
	}

	// FootRotation.Pitch = 0.f;
	FVector TraceStart(FootSocketLocation.X, FootSocketLocation.Y, ActorLocation.Z - CapsuleHalfHeight + TraceDistance);
	// FVector TraceStart(FootLocation.X, FootLocation.Y, ActorLocation.Z - CapsuleHalfHeight + TraceDistance);
	FVector TraceEnd = TraceStart - (TraceDistance + IkSettings->TraceDownwardExtend) * FVector::UpVector;
	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_IK);

	const float DownwardTraceExtend = 1.0f;
	FVector FootHalfSize(DownwardTraceExtend, IkSettings->FootLength * 0.5f, IkSettings->FootWidth);

	auto DebugDrawType = GetDrawDebugType();
	bool bHit = UKismetSystemLibrary::BoxTraceSingle(this, TraceStart, TraceEnd, FootHalfSize,
		FootRotation, TraceType, true,TArray<AActor*>(),
		DebugDrawType, HitResult, true);

	// bool bHit = UKismetSystemLibrary::LineTraceSingle(this, TraceStart, TraceEnd,
	// 	TraceType, true,TArray<AActor*>(),
	// 	EDrawDebugTrace::ForOneFrame, HitResult, true);
	//

	if (bHit)
	{
		float Elevation = (HitResult.Location.Z - (ActorLocation.Z - CapsuleHalfHeight) - DownwardTraceExtend / 2.f ) / IkData.IKScale;
		if (Elevation < IkSettings->MaxFootElevation)
			return Elevation;
	}

	return 0.f;
}

float UInverseKinematicsComponent::CalculateFootPitch(const USkeletalMeshComponent* SkeletalMesh, const FName& HeelSocketName, const FName& FootMiddleSocketName,
	const FName& ToesSocketName, const FName& FootSocketName, float PreviousValue)
{
	FVector HeelLocation = SkeletalMesh->GetSocketLocation(HeelSocketName);
	FVector ToesLocation = SkeletalMesh->GetSocketLocation(ToesSocketName);
	FVector FootMidLocation = SkeletalMesh->GetSocketLocation(FootMiddleSocketName);
	const FVector FootLocation = SkeletalMesh->GetBoneLocation(FootSocketName);
	float Correction = 1.f; // lifting up feet trace a little bit to prevent feet pitch jittering
	HeelLocation.Z = FootLocation.Z - FootOffset + Correction;
	ToesLocation.Z = FootLocation.Z - FootOffset + Correction;
	FootMidLocation.Z = FootLocation.Z - FootOffset + Correction;
	
	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_IK);

	const float MaxFootOffset = IkSettings->FootLength * 1.5f;
	auto DebugDrawType = GetDrawDebugType();
	
#pragma region box shape sweeps
	FVector FootHalfSize(IkSettings->FootLength / 10.f, IkSettings->FootLength / 10.f, 0.1f);

	bool bToesHit = UKismetSystemLibrary::BoxTraceSingle(this, ToesLocation,
		ToesLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, FootHalfSize, FRotator::ZeroRotator,
		TraceType,true,TArray<AActor*>(),
		DebugDrawType, HitResult, true);
	float ToesDistance = bToesHit ? FMath::Clamp(ToesLocation.Z - Correction - HitResult.Location.Z, 0.f, MaxFootOffset) : MaxFootOffset;
	
	bool bHeelHit = UKismetSystemLibrary::BoxTraceSingle(this, HeelLocation,
		HeelLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, FootHalfSize, FRotator::ZeroRotator,
		TraceType,true,TArray<AActor*>(),
		DebugDrawType, HitResult, true);
	float HeelDistance = bToesHit
		? bHeelHit
			? FMath::Clamp(HeelLocation.Z - Correction - HitResult.Location.Z, 0.f, MaxFootOffset)
			: MaxFootOffset
		: 0.f;

	bool bFootHit = UKismetSystemLibrary::BoxTraceSingle(this, FootMidLocation,
		FootMidLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, FootHalfSize, FRotator::ZeroRotator,
		TraceType,true,TArray<AActor*>(),
		DebugDrawType, HitResult, true);
	float FootDistance = bToesHit
		? bFootHit
			? FMath::Clamp(FootMidLocation.Z - Correction - HitResult.Location.Z, 0.f, MaxFootOffset)
			: 0.f
		: MaxFootOffset;
	
#pragma endregion box shape sweeps

#pragma region sphere shape sweeps
	// FCollisionQueryParams CollisionQueryParams;
	// CollisionQueryParams.AddIgnoredActor(GetOwner());
	// const float TraceRadius = IkSettings->FootLength / 5.f;
	// TraceUtils::FTraceParams TraceParams(true);
	
	// bool bHeelHit = TraceUtils::SweepSphereSingleByChannel(GetWorld(), HitResult,
	// 	HeelLocation, HeelLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, TraceRadius, ECC_IK,
	// 	CollisionQueryParams, TraceParams); 
	// float HeelDistance = bHeelHit ? FMath::Min(HeelLocation.Z - HitResult.Location.Z, MaxFootOffset) : MaxFootOffset;
	//
	// bool bToesHit = TraceUtils::SweepSphereSingleByChannel(GetWorld(), HitResult,ToesLocation,
	// 	ToesLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, TraceRadius,ECC_IK,CollisionQueryParams, TraceParams);
	// float ToesDistance = bToesHit ? FMath::Min(ToesLocation.Z - HitResult.Location.Z, MaxFootOffset) : 0.f;
	//
	// bool bFootHit = TraceUtils::SweepSphereSingleByChannel(GetWorld(), HitResult, FootMidLocation,
	// 	FootMidLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, TraceRadius, ECC_IK,CollisionQueryParams, TraceParams);
	// float FootDistance = bFootHit ? FMath::Min(FootMidLocation.Z - HitResult.Location.Z, MaxFootOffset) : 0.f;
#pragma endregion sphere shape sweeps

#pragma region line traces
	// bool bHeelHit = UKismetSystemLibrary::LineTraceSingle(this, HeelLocation,
	// 	HeelLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, TraceType,true,TArray<AActor*>(),
	// 	DebugDrawType, HitResult, true);
	// float HeelDistance = bHeelHit ? FMath::Min(HeelLocation.Z - HitResult.Location.Z, MaxFootOffset) : MaxFootOffset;

	// bool bToesHit = UKismetSystemLibrary::LineTraceSingle(this, ToesLocation,
	// 	ToesLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, TraceType,true,TArray<AActor*>(),
	// 	DebugDrawType, HitResult, true);
	// float ToesDistance = bToesHit ? FMath::Min(ToesLocation.Z - HitResult.Location.Z, MaxFootOffset) : 0.f;
	//
	// bool bFootHit = UKismetSystemLibrary::LineTraceSingle(this, FootMidLocation,
	// 	FootMidLocation - FVector::UpVector * IkSettings->TraceDownwardExtend, TraceType,true,TArray<AActor*>(),
	// 	DebugDrawType, HitResult, true);
	// float FootDistance = bFootHit ? FMath::Min(FootMidLocation.Z - HitResult.Location.Z, MaxFootOffset) : 0.f;
#pragma  endregion line traces
	
	// if (!(bHeelHit || bToesHit || bFootHit))
	// 	return 0.f;

	const float EnablePitchDistanceThreshold = 2.5f;
	// FMath::IsNearlyEqual(ToesDistance, HeelDistance, EnablePitchDistanceThreshold) || 
	if (FootDistance < EnablePitchDistanceThreshold
		&& (HeelDistance < EnablePitchDistanceThreshold || ToesDistance < EnablePitchDistanceThreshold))
	// if (HeelDistance < EnablePitchDistanceThreshold && ToesDistance < EnablePitchDistanceThreshold)
	{
		return 0.f;// PreviousValue;
	}

	// if there's space below heels and toes then just remain foot vertically?
	// if (HeelDistance > 2.f && ToesDistance > 2.f)
	// 	return 0.f;
	
	// 	if (FMath::IsNearlyEqual(HeelDistance -FootDistance, 0.f, 2.5f)
	// 	|| FMath::IsNearlyEqual(ToesDistance-FootDistance, 0.f, 2.5f))
	// {
	// 	return 0.f;//PreviousValue;
	// }
	
	float AngleAdjustmentFactor = 1.75; 
	float AtanRad = FMath::Atan(FMath::Abs(ToesDistance - HeelDistance)/(IkSettings->FootLength * IkData.IKScale));
	float FootDeclineAngle = FMath::RadiansToDegrees(AtanRad);
	float ClampedAngle =  FootDeclineAngle;// FMath::ClampAngle(FootDeclineAngle, 0.f, 30.f) * AngleAdjustmentFactor;
	if (FMath::Abs(ClampedAngle) > 10.f)
	{
		int FUCK = 1;
	}
	return HeelDistance < ToesDistance ? ClampedAngle : -ClampedAngle;
}

EDrawDebugTrace::Type UInverseKinematicsComponent::GetDrawDebugType() const
{
	
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	bool bDebugEnabled = GetDebugSubsystem()->IsDebugCategoryEnabled(DebugCategoryIK);
#else
	bool bDebugEnabled = false;
#endif

	return bDebugEnabled ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
}

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG

const UDebugSubsystem* UInverseKinematicsComponent::GetDebugSubsystem() const
{
	if (!IsValid(DebugSubsystem))
		DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();

	return DebugSubsystem;
}

#endif

