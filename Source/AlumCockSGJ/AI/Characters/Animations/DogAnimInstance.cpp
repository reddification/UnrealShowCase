#include "DogAnimInstance.h"

#include "AI/Characters/AIDogCharacter2.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDogAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	DogCharacter = StaticCast<AAIDogCharacter2*>(TryGetPawnOwner());
}

void UDogAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!DogCharacter)
		return;

	Speed = DogCharacter->GetCharacterMovement()->Velocity.Size();
	DogAction = DogCharacter->GetCurrentAction();
}

void UDogAnimInstance::ApplyState(const UBaseNpcStateSettings* NpcState)
{
	Super::ApplyState(NpcState);
}
