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
            //j'ai add ca aussi
            "UMG",
            "Slate",
            "SlateCore",
            "EditorScriptingUtilities",
            //
            "EnhancedInput",
            "LocalMultiplayer",
            "DeveloperSettings"
        });

        //j'ajoute ca les bros
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",
            "LevelEditor",
            "EditorFramework",
            "Slate",
            "SlateCore",
            "EditorSubsystem",
            "Blutility",
            "Projects"
        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
                "LevelEditor",
                "Slate",
                "SlateCore",
                "EditorFramework",
                "EditorSubsystem",
                "CoreUObject",
                "Engine"
            });
        }
        //jusqu'ici
        //jtm Gaetan mon gatté GAETTé :^)
    }
}
