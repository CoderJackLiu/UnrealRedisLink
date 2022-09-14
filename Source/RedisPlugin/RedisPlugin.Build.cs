// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class RedisPlugin : ModuleRules
{
	public RedisPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableUndefinedIdentifierWarnings = true;

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        string ThirdPartyPath = "../ThirdParty/";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, ThirdPartyPath + "hiredis/redis-win/deps/hiredis"));
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, ThirdPartyPath + "hiredis/redis-win/deps"));

            PublicSystemLibraryPaths.Add(Path.Combine(ModuleDirectory, ThirdPartyPath + "lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, ThirdPartyPath + "lib/Win64/hiredis.lib"));


        }
        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, ThirdPartyPath + "hiredis"));
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, ThirdPartyPath));

            PublicSystemLibraryPaths.Add(Path.Combine(ModuleDirectory, ThirdPartyPath + "lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, ThirdPartyPath + "lib/Linux/libhiredis.a"));
        }
	}
}
