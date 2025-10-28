#include "SMultiHandleSlider.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

void SMultiHandleSlider::Construct(const FArguments& InArgs)
{
	Orientation = InArgs._Orientation;
	BarThickness = InArgs._BarThickness;
	HandleRadius = InArgs._HandleRadius;
	StepSize = InArgs._StepSize;
	ValuesAttr = InArgs._Values01;
	OnValuesChanged = InArgs._OnValuesChanged;

	Values = ValuesAttr.Get();
	Values.Sort();
}

int32 SMultiHandleSlider::OnPaint(const FPaintArgs& Args, const FGeometry& Geo,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& Out, int32 LayerId,
	const FWidgetStyle& InStyle, bool bParentEnabled) const
{
	TArray<float> Current = ValuesAttr.IsBound() ? ValuesAttr.Get() : Values;
	TArray<float> Sorted = Current; Sorted.Sort();

	const FSlateBrush* Brush = FCoreStyle::Get().GetBrush("WhiteBrush");

	const FVector2D Size = Geo.GetLocalSize();
	const float TrackHalf = BarThickness * 0.5f;

	FVector2D TrackStart, TrackEnd;
	if (Orientation == Orient_Horizontal)
	{
		TrackStart = FVector2D(HandleRadius, 0.5f * Size.Y);
		TrackEnd = FVector2D(Size.X - HandleRadius, 0.5f * Size.Y);

		FSlateDrawElement::MakeBox(
			Out, ++LayerId, Geo.ToPaintGeometry(
				FVector2D(0.f, 0.5f * Size.Y - TrackHalf),
				FVector2D(Size.X, BarThickness)),
			Brush, ESlateDrawEffect::None, FLinearColor(0.25f, 0.25f, 0.25f, 1.f));

		for (int32 i = 0; i + 1 < Sorted.Num(); i += 2)
		{
			const float X0 = Value01ToPixel(Geo, Sorted[i]);
			const float X1 = Value01ToPixel(Geo, Sorted[i + 1]);
			FSlateDrawElement::MakeBox(
				Out, ++LayerId, Geo.ToPaintGeometry(
					FVector2D(X0, 0.5f * Size.Y - TrackHalf),
					FVector2D(FMath::Max(1.f, X1 - X0), BarThickness)),
				Brush, ESlateDrawEffect::None, FLinearColor(0.1f, 0.6f, 1.f, 0.6f));
		}

		for (float V : Current)
		{
			const float X = Value01ToPixel(Geo, V);
			const FVector2D P = FVector2D(X - HandleRadius, 0.5f * Size.Y - HandleRadius);
			const FVector2D S = FVector2D(HandleRadius * 2.f, HandleRadius * 2.f);
			FSlateDrawElement::MakeBox(Out, ++LayerId, Geo.ToPaintGeometry(P, S),
				Brush, ESlateDrawEffect::None, FLinearColor::White);
		}
	}
	else
	{
		FSlateDrawElement::MakeBox(
			Out, ++LayerId, Geo.ToPaintGeometry(
				FVector2D(0.5f * Size.X - TrackHalf, 0.f),
				FVector2D(BarThickness, Size.Y)),
			Brush, ESlateDrawEffect::None, FLinearColor(0.25f, 0.25f, 0.25f, 1.f));

		for (int32 i = 0; i + 1 < Sorted.Num(); i += 2)
		{
			const float Y1 = Value01ToPixel(Geo, Sorted[i]);
			const float Y0 = Value01ToPixel(Geo, Sorted[i + 1]);
			FSlateDrawElement::MakeBox(
				Out, ++LayerId, Geo.ToPaintGeometry(
					FVector2D(0.5f * Size.X - TrackHalf, Y0),
					FVector2D(BarThickness, FMath::Max(1.f, Y1 - Y0))),
				Brush, ESlateDrawEffect::None, FLinearColor(0.1f, 0.6f, 1.f, 0.6f));
		}

		for (float V : Current)
		{
			const float Y = Value01ToPixel(Geo, V);
			const FVector2D P = FVector2D(0.5f * Size.X - HandleRadius, Y - HandleRadius);
			const FVector2D S = FVector2D(HandleRadius * 2.f, HandleRadius * 2.f);
			FSlateDrawElement::MakeBox(Out, ++LayerId, Geo.ToPaintGeometry(P, S),
				Brush, ESlateDrawEffect::None, FLinearColor::White);
		}
	}

	return LayerId;
}

FReply SMultiHandleSlider::OnMouseButtonDown(const FGeometry& Geo, const FPointerEvent& E)
{
	if (E.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();
	ActiveIndex = HitTestHandle(Geo, E.GetScreenSpacePosition());
	if (ActiveIndex == INDEX_NONE)
	{
		// If clicked empty space, create or move nearest handle (optional behavior)
	}
	FSlateApplication::Get().SetKeyboardFocus(AsShared());
	return FReply::Handled().CaptureMouse(AsShared());
}

FReply SMultiHandleSlider::OnMouseButtonUp(const FGeometry&, const FPointerEvent& E)
{
	if (E.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();
	ActiveIndex = INDEX_NONE;
	return FReply::Handled().ReleaseMouseCapture();
}

FReply SMultiHandleSlider::OnMouseMove(const FGeometry& Geo, const FPointerEvent& E)
{
	if (!HasMouseCapture() || ActiveIndex == INDEX_NONE) return FReply::Unhandled();

	const FVector2D Local = Geo.AbsoluteToLocal(E.GetScreenSpacePosition());
	float V01 = (Orientation == Orient_Horizontal)
		? PixelToValue01(Geo, Local.X)
		: PixelToValue01(Geo, Local.Y);

	V01 = FMath::Clamp(Snap(V01), 0.f, 1.f);

	const float Eps = 0.001f;
	Values = ValuesAttr.IsBound() ? ValuesAttr.Get() : Values;
	Values[ActiveIndex] = V01;
	Values.Sort();

	int32 NewIndex = 0;
	float BestDist = FLT_MAX;
	for (int32 i = 0; i < Values.Num(); ++i)
	{
		float D = FMath::Abs(Values[i] - V01);
		if (D < BestDist) { BestDist = D; NewIndex = i; }
	}
	ActiveIndex = NewIndex;

	if (Values.Num() > 1)
	{
		if (ActiveIndex > 0)        Values[ActiveIndex] = FMath::Max(Values[ActiveIndex], Values[ActiveIndex - 1] + Eps);
		if (ActiveIndex < Values.Num() - 1) Values[ActiveIndex] = FMath::Min(Values[ActiveIndex], Values[ActiveIndex + 1] - Eps);
	}

	if (OnValuesChanged.IsBound()) OnValuesChanged.Execute(Values);

	return FReply::Handled();
}

float SMultiHandleSlider::PixelToValue01(const FGeometry& Geo, float Pixel) const
{
	const FVector2D Size = Geo.GetLocalSize();
	if (Orientation == Orient_Horizontal)
	{
		const float Min = HandleRadius;
		const float Max = Size.X - HandleRadius;
		return (Pixel - Min) / FMath::Max(1.f, (Max - Min));
	}
	else
	{
		const float Min = HandleRadius;
		const float Max = Size.Y - HandleRadius;
		return (Pixel - Min) / FMath::Max(1.f, (Max - Min));
	}
}

float SMultiHandleSlider::Value01ToPixel(const FGeometry& Geo, float V01) const
{
	const FVector2D Size = Geo.GetLocalSize();
	if (Orientation == Orient_Horizontal)
	{
		const float Min = HandleRadius;
		const float Max = Size.X - HandleRadius;
		return FMath::Lerp(Min, Max, V01);
	}
	else
	{
		const float Min = HandleRadius;
		const float Max = Size.Y - HandleRadius;
		return FMath::Lerp(Min, Max, V01);
	}
}

int32 SMultiHandleSlider::HitTestHandle(const FGeometry& Geo, FVector2D Cursor) const
{
	const FVector2D Local = Geo.AbsoluteToLocal(Cursor);
	const TArray<float> Curr = ValuesAttr.IsBound() ? ValuesAttr.Get() : Values;

	int32 BestIdx = INDEX_NONE;
	float Best = FLT_MAX;

	for (int32 i = 0; i < Curr.Num(); ++i)
	{
		const float P = (Orientation == Orient_Horizontal)
			? Value01ToPixel(Geo, Curr[i]) - Local.X
			: Value01ToPixel(Geo, Curr[i]) - Local.Y;
		const float Dist = FMath::Abs(P);
		if (Dist < Best && Dist <= HandleRadius * 1.5f) { Best = Dist; BestIdx = i; }
	}
	return BestIdx;
}

float SMultiHandleSlider::Snap(float V) const
{
	if (StepSize > KINDA_SMALL_NUMBER && StepSize > 0.f)
	{
		const float Steps = FMath::RoundToFloat(V / StepSize);
		return FMath::Clamp(Steps * StepSize, 0.f, 1.f);
	}
	return V;
}
