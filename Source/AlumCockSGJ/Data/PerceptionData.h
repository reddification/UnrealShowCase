#pragma once

struct FPerceptionData
{
    TWeakObjectPtr<AActor> ClosestObservedActor;
    TWeakObjectPtr<AActor> MostDamagingActor;
    float MostDamagingActorDamage = 0.f;
};
