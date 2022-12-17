// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PortalGunDemo : ModuleRules
{
	public PortalGunDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
