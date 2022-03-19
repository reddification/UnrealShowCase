#pragma once

#include "CoreMinimal.h"

#define ECC_Climbable ECC_GameTraceChannel1
#define ECC_InteractableOverlap ECC_GameTraceChannel2
#define ECC_Wallrunnable ECC_GameTraceChannel3
#define ECC_Bullet ECC_GameTraceChannel4
#define ECC_MeleeHitRegistrator ECC_GameTraceChannel5
#define ECC_Projectile ECC_GameTraceChannel6
#define ECC_Wind ECC_GameTraceChannel7
#define ECC_InteractableVision ECC_GameTraceChannel8
#define ECC_IK ECC_GameTraceChannel9

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryIK = FName("IK");
const FName DebugCategoryWallrun = FName("Wallrun");
const FName DebugCategoryZipline = FName("Zipline");
const FName DebugCategoryAttributes = FName("Attributes");
const FName DebugCategoryRangeWeapons = FName("RangeWeapons");
const FName DebugCategoryMeleeWeapons = FName("MeleeWeapons");

const FName ProfilePawn = FName("Pawn");
const FName ProfileRagdoll = FName("Ragdoll");
const FName ProfileWorldItem = FName("WorldItem");
const FName ProfileInteractionVolume = FName("PawnInteractionVolume");
const FName ProfileProjectile = FName("Projectile");
const FName ProfileNoCollision = FName("NoCollision");
const FName ProfileNoCollisionInteraction = FName("NoCollisionInteraction");
const FName ProfileCharacterCapsule = FName("CharacterCapsule");

const FName MontageSectionBeginLoop = FName("BeginLoop");
const FName MontageSectionEndLoop = FName("EndLoop");

// TODO move it somewhere to be configurable?
const FName SocketPistol = FName("hand_r_pistol_socket"); 
const FName SocketAssaultRifle = FName("hand_r_assault_rifle_socket"); 
const FName SocketForegrip = FName("foregrip_socket");
const FName SocketCarriedItemTwoHandedLight = FName("CarriedItemSocket");
const FName SocketEyesViewpoint = FName("EyeViewpointSocket");
const FName AISocketDefaultCharacterTarget = FName("ai_target_socket");

const FName FXParamTraceEnd = FName("TraceEnd");

const FName BB_CurrentTarget = FName("CurrentTarget");
const FName BB_CurrentTargetLastLocation = FName("CurrentTargetLastLocation");
const FName BB_CombatMode = FName("CombatMode");

const FName BB_NextLocation = FName("NextLocation");
const FName BB_IsPatrolling = FName("IsPatrolling");
const FName BB_FocusPoint = FName("FocusPoint");
const FName BB_FocusActor = FName("FocusActor");

const FName BB_InterestingLocation = FName("InterestingLocation");

const FName BB_ActivityCoreLocationVector = FName("ActivityCoreLocationVector");
const FName BB_RememberedLocation = FName("RememberedLocation");
const FName BB_bKnowActivtyFact1 = FName("bKnowActivityFact1");
const FName BB_ActivityItemsCount = FName("ActivityItemsCount");
const FName BB_IterationsCount = FName("IterationsCount");
const FName BB_ActorToInteract = FName("ActorToInteract");
const FName BB_WorkUtility = FName("WorkUtility");
const int BB_UnsetInteger = -1;
const FName BB_bWantToInteract = FName("bWantToInteract");
const FName BB_bInDanger = FName("bInDanger");
const FName BB_Anxiety = FName("Anxiety");
const FName BB_AnxietyState = FName("AnxietyState");
const FName BB_Interest = FName("Interest");
const FName BB_InterestState = FName("InterestState");
const FName BB_RunningActivityType = FName("RunningActivityType");
const FName BB_ActorInteractionState = FName("ActorInteractionState");
const FName BB_StopInteractionDelay = FName("StopInteractionDelay");

const FName BB_BoolParameter1 = FName("BoolParameter1");
const FName BB_BoolParameter2 = FName("BoolParameter2");

// const FName BB_bAnxious = FName("bAnxious");
// const FName BB_bInterested = FName("bInterested");


// const FName BB_MostDangerousTarget = FName("MostDangerousTarget");
// const FName BB_MostDangerousTargetDamage = FName("MostDangerousTargetDamage");

const FName CharacterAttributesComponentName(TEXT("Attributes"));

const FName SoundStimulusTag_Shot = FName("Shot");
const FName SoundStimulusTag_Explosion = FName("Explosion");
const FName SoundStimulusTag_Speech = FName("Speech");
const FName SoundStimulusTag_Step = FName("Step");

