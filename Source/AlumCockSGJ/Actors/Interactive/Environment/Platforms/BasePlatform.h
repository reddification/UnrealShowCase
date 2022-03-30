#pragma once

#include "CoreMinimal.h"

#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BasePlatform.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ABasePlatform : public AActor
{
	GENERATED_BODY()

public:	
	ABasePlatform();

	UFUNCTION(BlueprintCallable)
	bool IsMoving() const { return PlatformTimeline.IsPlaying(); }

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BottomCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* PlatformTimelineCurve;

	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation = FVector::ZeroVector;
	
	UFUNCTION(BlueprintCallable, Category=Platform)
	void Start();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageOnCollision = 20.f;

	virtual void OnPlatformReachedFinalPosition();

	FTimeline PlatformTimeline;

    bool bActive = false;
	bool bReverse = false;

private:

	void SetPlatformPosition(float Alpha);

	UFUNCTION()
	void OnBottomCollisionOverlapped(UPrimitiveComponent* PrimitiveComponent, AActor* Actor, UPrimitiveComponent* OtherActorComponent, int OtherBodyIndex,
		bool bFromSweep, const FHitResult& HitResult);
};