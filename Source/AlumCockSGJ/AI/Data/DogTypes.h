// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DogTypes.generated.h"

UENUM(BlueprintType, Category = "AI Dog")
enum class EDogBehaviorState : uint8
{
    Idle = 0 UMETA(DisplayName = "Idle"),
    FollowThePlayer = 1 UMETA(DisplayName = "FollowThePlayer"),
    PlayerFollowTheDog = 2 UMETA(DisplayName = "PlayerFollowTheDog"),
};

UENUM(BlueprintType, Category = "AI Dog")
enum class EDogAction : uint8
{
    None = 0 UMETA(DisplayName = "None"),
    Sniffs = 1 UMETA(DisplayName = "Sniffs"),
    Digging = 2 UMETA(DisplayName = "Digging"),
    Peeing = 3 UMETA(DisplayName = "Peeing"),
    Barks = 4 UMETA(DisplayName = "Barks")
};