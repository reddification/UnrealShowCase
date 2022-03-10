#pragma once

#include "CoreMinimal.h"

#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BasePlatform.generated.h"

UENUM()
enum class EPlatformBehavior : uint8
{
	OnDemand = 0,
	Loop,
	ActivatedByGenerator
};

UCLASS()
class ALUMCOCKSGJ_API ABasePlatform : public AActor
{
	GENERATED_BODY()

public:	
	ABasePlatform();

	UFUNCTION(BlueprintCallable)
	bool IsMoving() const { return PlatformTimeline.IsPlaying(); }
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BottomCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category=Platform)
	FVector StartLocation = FVector::ZeroVector;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category=Platform, meta = (MakeEditWidget))
	FVector EndLocation;

	//Set defaults only since it should be editable only in blueprints class defaults, otherwise behavior is kinda unpredictable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Platform)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;
	
	/**
	 * Sets delay in seconds between reversing platform direction when Loop behavior is set
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Platform)
	float PlatformLoopDelay = 2.0f;

	/*
	 * Sets delay in seconds between resetting platform position when Generator behavior is set
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Platform)
	float PlatformResetDelay = 1.0f;
	
	/*
	 * Is platform initially active
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Platform)
	bool bActive = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category=Platform)
	class APlatformInvocator* PlatformInvocator;

	/*
	 * Platform invocators event handler
	 */
	UFUNCTION()
	void OnPlatformInvoked();

	/*
	 * Starts platform movement
	 */
	UFUNCTION(BlueprintCallable, Category=Platform)
	void Start();

	/*
	 * Stops platform movement and makes platform go reverse on next start
	 */
	UFUNCTION(BlueprintCallable, Category=Platform)
	void ReverseStop();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageOnCollision = 20.f;
	
public:	
	virtual void Tick(float DeltaTime) override;

	// alternative approach - timeline float curve with whole path (up and down) so that you can set SetLooping(true)?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Platform)
	UCurveFloat* PlatformTimelineCurve;
	
private:
	FTimeline PlatformTimeline;
	FTimerHandle PlatformLoopDelayTimer;
	FTimerHandle PlatformReturnDelayTimer;

	void ResetPlatform();
	void ResumePlatform();
	void SetPlatformPosition(float Alpha);
	void OnPlatformReachedFinalPosition();
	bool bReverse = false;

	UFUNCTION()
	void OnBottomCollisionOverlapped(UPrimitiveComponent* PrimitiveComponent, AActor* Actor, UPrimitiveComponent* OtherActorComponent, int OtherBodyIndex,
		bool bFromSweep, const FHitResult& HitResult);
};