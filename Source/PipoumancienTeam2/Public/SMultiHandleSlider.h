#pragma once
#include "Widgets/SLeafWidget.h"
#include "Input/Reply.h"

DECLARE_DELEGATE_OneParam(FOnMultiSliderChanged, const TArray<float>&);

class SMultiHandleSlider : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SMultiHandleSlider)
		: _Orientation(Orient_Horizontal)
		, _BarThickness(4.f)
		, _HandleRadius(6.f)
		, _StepSize(0.f)
		{
		}
		SLATE_ARGUMENT(EOrientation, Orientation)
		SLATE_ARGUMENT(float, BarThickness)
		SLATE_ARGUMENT(float, HandleRadius)
		SLATE_ARGUMENT(float, StepSize)
			SLATE_ATTRIBUTE(TArray<float>, Values01)
			SLATE_EVENT(FOnMultiSliderChanged, OnValuesChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D(160.f, 28.f); }
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
		int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual bool SupportsKeyboardFocus() const override { return true; }

private:
	float PixelToValue01(const FGeometry& Geo, float Pixel) const;
	float Value01ToPixel(const FGeometry& Geo, float V01) const;
	int32 HitTestHandle(const FGeometry& Geo, FVector2D Cursor) const;
	float Snap(float V) const;

private:
	TArray<float> Values;
	TAttribute<TArray<float>> ValuesAttr;

	EOrientation Orientation = Orient_Horizontal;
	float BarThickness = 4.f;
	float HandleRadius = 6.f;
	float StepSize = 0.f;

	mutable int32 ActiveIndex = INDEX_NONE;
	FOnMultiSliderChanged OnValuesChanged;
};
