// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EverWrestle : ModuleRules
{
	public EverWrestle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"AdvancedSessions",
			"AdvancedSteamSessions",
			"EngineSettings",
			"Steamworks"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			"EverWrestle",
			"EverWrestle/Variant_Platforming",
			"EverWrestle/Variant_Platforming/Animation",
			"EverWrestle/Variant_Combat",
			"EverWrestle/Variant_Combat/AI",
			"EverWrestle/Variant_Combat/Animation",
			"EverWrestle/Variant_Combat/Gameplay",
			"EverWrestle/Variant_Combat/Interfaces",
			"EverWrestle/Variant_Combat/UI",
			"EverWrestle/Variant_SideScrolling",
			"EverWrestle/Variant_SideScrolling/AI",
			"EverWrestle/Variant_SideScrolling/Gameplay",
			"EverWrestle/Variant_SideScrolling/Interfaces",
			"EverWrestle/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
