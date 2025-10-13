using UnrealBuildTool;

public class LocalMultiplayer: ModuleRules
{
    public LocalMultiplayer(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "DeveloperSettings"});
    }
}
