// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ExperimentalFPS : ModuleRules
{
	public ExperimentalFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("ExperimentalFPS");

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
