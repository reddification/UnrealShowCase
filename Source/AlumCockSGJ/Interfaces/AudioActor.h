#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AudioActor.generated.h"

UINTERFACE()
class UAudioActor : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IAudioActor
{
	GENERATED_BODY()

public:
	virtual float PlaySound(class USoundCue* Sound) { return 0.f; }
};
