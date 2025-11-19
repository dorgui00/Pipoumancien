#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "MultiHandleSlider.generated.h"

class SMultiHandleSlider;

USTRUCT(BlueprintType)
struct FMultiSliderHandleSorted
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	float Distance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	float SpacingFromPrevious = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	int32 Lane = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	FName InputKey = NAME_None;
};

USTRUCT(BlueprintType)
struct FMultiSliderLane
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	TArray<float> Values01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	TArray<FName> Keys;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiSliderValuesChanged, const TArray<float>&, NewValues);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiSliderLanesChangedBP, const TArray<FMultiSliderLane>&, NewLanes);

UCLASS(meta = (DisplayName = "Multi-Handle Slider"))
class PIPOUMANCIENTEAM2_API UMultiHandleSlider : public UWidget
{
	GENERATED_BODY()

public:
//#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, Category = "Slider")
	void GetSortedHandles(TArray<FMultiSliderHandleSorted>& OutSorted) const;
//#endif
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	TEnumAsByte<EOrientation> Orientation = Orient_Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float BarThickness = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float HandleRadius = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StepSize = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider", meta = (ClampMin = "1", UIMin = "1"))
	int32 NumLanes = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
	TArray<FMultiSliderLane> Lanes;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMultiSliderValuesChanged OnValuesChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMultiSliderLanesChangedBP OnLanesChanged;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	//blueprints

	UFUNCTION(BlueprintPure, Category = "Slider|Data")
	TArray<FMultiSliderLane> GetAllLanes() const;

	UFUNCTION(BlueprintPure, Category = "Slider|Data")
	TArray<FMultiSliderHandleSorted> GetAllHandlesSorted() const;

	UFUNCTION(BlueprintCallable, Category = "Slider|Data")
	void SetAllLanes(const TArray<FMultiSliderLane>& NewLanes);

	UFUNCTION(BlueprintCallable, Category = "Slider|Data")
	void SetKeysForLane(int32 LaneIndex, const TArray<FName>& NewKeys);

	UFUNCTION(BlueprintCallable, Category = "Slider|Data")
	void SetKeyAt(int32 LaneIndex, int32 HandleIndex, FName NewKey);

	UFUNCTION(BlueprintCallable, Category = "Slider|Data")
	void ClearAllKeys();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;

#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual const FText GetPaletteCategory() override { return NSLOCTEXT("UMG", "Inputs", "Inputs"); }
#endif

private:
	TSharedPtr<SMultiHandleSlider> MySlateWidget;

	void Slate_OnValuesChanged(const TArray<float>& NewLane0);
	void Slate_OnLanesChanged(const TArray<TArray<float>>& NewAll);

	void BuildSlateLaneArray(TArray<TArray<float>>& Out) const;
	void ApplySlateLaneArray(const TArray<TArray<float>>& In);


	//blueprint

	float SnapToStep(float X) const;

	static void SortLaneKeepKeys(FMultiSliderLane& Lane);

	void PushToSlate();

};
