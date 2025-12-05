// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PipoumancienTeam2 : ModuleRules
{
    public PipoumancienTeam2(ReadOnlyTargetRules Target) : base(Target)
    {
        
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",
            "SlateCore",
            "EnhancedInput",
            "LocalMultiplayer",
            "DeveloperSettings",
                
            "NavigationSystem",
            "AIModule", 
            "NiagaraUIRenderer"
        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
                "LevelEditor",
                "EditorFramework",
                "EditorSubsystem",
                "Blutility",
                "Projects",
                "EditorScriptingUtilities",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "NiagaraUIRendererEditor"
            });
        }
        //jusqu'ici
    }
}
