#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Movement/IKData.h"
#include "Kismet/KismetSystemLibrary.h"

#include "InverseKinematicsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALUMCOCKSGJ_API UInverseKinematicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void CalculateIkData(const USkeletalMeshComponent* SkeletalMesh, float CapsuleHalfHeight, FVector ActorLocation,
		bool bCrouched, float DeltaTime);

	const FIkData& GetIkData() const { return IkData; }
	void SetScale(float Scale) { IkData.IKScale = Scale; }
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UIKSettings* IkSettings;

private:
	FIkData IkData;
	bool bInitialized = false;
	
	void RecalculateFeetPitches(const USkeletalMeshComponent* SkeletalMesh, float DeltaSeconds);
	void RecalculateKneesExtends(float DeltaSeconds);
	void UpdateLegsIkOffsetsBoxTrace(const USkeletalMeshComponent* SkeletalMesh, float CapsuleHalfHeight,
		const FVector& ActorLocation, bool bCrouched, float DeltaSeconds);
	void RecalculateFeetElevationsWithPelvis();
	float GetIkElevationForSocket(const FName& SocketName, const FName& FootBoneName,
	                              const USkeletalMeshComponent* SkeletalMesh, const FVector& ActorLocation, float CapsuleHalfHeight, bool bCrouched);
	float CalculateFootPitch(const USkeletalMeshComponent* SkeletalMesh, const FName& HeelSocketName, const FName& FootMiddleSocketName,
	                         const FName& ToesSocketName, const FName& FootSocketName, float PreviousValue);

	EDrawDebugTrace::Type GetDrawDebugType() const;

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	mutable class UDebugSubsystem* DebugSubsystem = nullptr;
	const UDebugSubsystem* GetDebugSubsystem() const;
#endif
	
};
