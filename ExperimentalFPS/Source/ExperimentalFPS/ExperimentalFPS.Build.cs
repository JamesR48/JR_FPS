// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ExperimentalFPS : ModuleRules
{
	public ExperimentalFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
