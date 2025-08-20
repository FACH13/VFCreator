// Copyright (C) 2019-2020 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class VFCreator : ModuleRules
{
	public VFCreator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);

		if(Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "DetailCustomizations", "PropertyEditor", "EditorStyle"});
			PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
		}
	}
}
