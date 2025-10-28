
#pragma once
#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "MultiHandleSlider.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiSliderValuesChanged, const TArray<float>&, NewValues);

UCLASS(meta = (DisplayName = "Multi-Handle Slider"))
class PIPOUMANCIENTEAM2_API  UMultiHandleSlider : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	TArray<float> Values01{ 0.25f, 0.75f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	TEnumAsByte<EOrientation> Orientation = Orient_Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float BarThickness = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float HandleRadius = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StepSize = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMultiSliderValuesChanged OnValuesChanged;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override { return NSLOCTEXT("UMG", "Inputs", "Inputs"); }
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;

private:
	TSharedPtr<class SMultiHandleSlider> MySlateWidget;
	void Slate_OnValuesChanged(const TArray<float>& NewValues);
};
