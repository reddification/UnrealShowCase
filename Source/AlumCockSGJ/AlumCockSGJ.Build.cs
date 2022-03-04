// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AlumCockSGJ : ModuleRules
{
	public AlumCockSGJ(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "BTUtilityPlugin", "Slate", "SlateCore", "ImageCore" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "UMG", 
			"AIModule",
			"NavigationSystem",
			"GameplayTags",
			"GameplayTasks",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"FMODStudio",
			"LevelSequence",
			"MovieScene",
			
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		PrivateIncludePaths.AddRange(new []{ Name });
	}
}
