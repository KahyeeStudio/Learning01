// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectScavenger : ModuleRules
{
	public ProjectScavenger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core",
			"CoreUObject",
			"Engine",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// 输入相关：一般只在cpp里包含。
			"InputCore",
			"EnhancedInput",
			// UI：通常不会在对外头文件里直接暴露SWidget/UUserWidget。
			"UMG",
			"Slate",
			"SlateCore",
			"ApplicationCore",
			// 如果以后引入Lyra风格模块，可在这里逐个解注释。
			"ModularGameplay",
			//"ModularGameplayActors",
			"GameFeatures",
			"DataRegistry",
			//"CommonGame",
			//"GameplayMessageRuntime",
		});

		PublicIncludePaths.AddRange(new string[] {
			"ProjectScavenger",
			"ProjectScavenger/Variant_Strategy",
			"ProjectScavenger/Variant_Strategy/UI",
			"ProjectScavenger/Variant_TwinStick",
			"ProjectScavenger/Variant_TwinStick/AI",
			"ProjectScavenger/Variant_TwinStick/Gameplay",
			"ProjectScavenger/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
