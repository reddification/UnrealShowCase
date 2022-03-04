#include "CameraShakeComponent.h"

UCameraShakeComponent::UCameraShakeComponent()
{
    OscillationDuration = OscillationDurationValue;
    OscillationBlendInTime = OscillationBlendInTimeValue;
    OscillationBlendOutTime = OscillationBlendOutTimeValue;

    RotOscillation.Pitch.Amplitude = RotOscillationPitchAmplitudeValue;
    RotOscillation.Pitch.Frequency = FMath::RandRange(1.5f, 2.0f);

    RotOscillation.Yaw.Amplitude = RotOscillationYawAmplitudeValue;
    RotOscillation.Yaw.Frequency = FMath::RandRange(1.5f, 2.0f);

    LocOscillation.X.Amplitude = LocOscillationXAmplitudeValue;
    LocOscillation.X.Frequency = LocOscillationXFrequencyValue;

    LocOscillation.Z.Amplitude = LocOscillationZAmplitudeValue;
    LocOscillation.Z.Frequency = LocOscillationZFrequencyValue;
}
