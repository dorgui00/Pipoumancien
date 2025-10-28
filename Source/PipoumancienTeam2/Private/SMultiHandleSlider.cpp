#include "SMultiHandleSlider.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Framework/Application/SlateApplication.h"

void SMultiHandleSlider::Construct(const FArguments& InArgs)
{
	Orientation = InArgs._Orientation;
	BarThickness = InArgs._BarThickness;
	HandleRadius = InArgs._HandleRadius;
	StepSize = InArgs._StepSize;

	ValuesAttr = InArgs._Values01;

	LaneValues = InArgs._LaneValues;
	if (LaneValues.Num() == 0)
	{
		Values = ValuesAttr.IsBound() ? ValuesAttr.Get() : Values;
		Values.Sort();
		LaneValues.Add(Values);
	}
	NumLanes = LaneValues.Num();
	for (TArray<float>& L : LaneValues) { L.Sort(); }

	OnValuesChanged = InArgs._OnValuesChanged;
	OnLanesChanged = InArgs._OnLanesChanged;
}

void SMultiHandleSlider::SetLaneValues(const TArray<TArray<float>>& In)
{
	LaneValues = In;
	NumLanes = LaneValues.Num();
	for (TArray<float>& L : LaneValues) { L.Sort(); }
}

int32 SMultiHandleSlider::OnPaint(const FPaintArgs& Args, const FGeometry& Geo,
	const FSlateRect& Cull, FSlateWindowElementList& Out, int32 LayerId,
	const FWidgetStyle& Style, bool bParentEnabled) const
{
	const FSlateBrush* Brush = FCoreStyle::Get().GetBrush("WhiteBrush");
	const FVector2D Size = Geo.GetLocalSize();

	const float TrackThickness = FMath::Max(1.f, BarThickness);
	const FLinearColor TrackColor(0.25f, 0.25f, 0.25f, 1.f);

	for (int32 Lane = 0; Lane < NumLanes; ++Lane)
	{
		if (Orientation == Orient_Horizontal)
		{
			const float Y = LanePosY(Geo, Lane);
			const FVector2D P(HandleRadius, Y - 0.5f * TrackThickness);
			const FVector2D S(Size.X - 2.f * HandleRadius, TrackThickness);
			FSlateDrawElement::MakeBox(Out, ++LayerId, Geo.ToPaintGeometry(P, S), Brush, ESlateDrawEffect::None, TrackColor);
		}
		else
		{
			const float X = LanePosX(Geo, Lane);
			const FVector2D P(X - 0.5f * TrackThickness, HandleRadius);
			const FVector2D S(TrackThickness, Size.Y - 2.f * HandleRadius);
			FSlateDrawElement::MakeBox(Out, ++LayerId, Geo.ToPaintGeometry(P, S), Brush, ESlateDrawEffect::None, TrackColor);
		}
	}

	for (int32 Lane = 0; Lane < LaneValues.Num(); ++Lane)
	{
		const TArray<float>& Curr = LaneValues[Lane];

		if (Orientation == Orient_Horizontal)
		{
			const float Y = LanePosY(Geo, Lane);
			for (float V : Curr)
			{
				const float X = Value01ToPixel(Geo, V);
				const FVector2D P(X - HandleRadius, Y - HandleRadius);
				const FVector2D S(HandleRadius * 2.f, HandleRadius * 2.f);
				FSlateDrawElement::MakeBox(Out, ++LayerId, Geo.ToPaintGeometry(P, S), Brush, ESlateDrawEffect::None, FLinearColor::White);
			}
		}
		else
		{
			const float X = LanePosX(Geo, Lane);
			for (float V : Curr)
			{
				const float Y = Value01ToPixel(Geo, V);
				const FVector2D P(X - HandleRadius, Y - HandleRadius);
				const FVector2D S(HandleRadius * 2.f, HandleRadius * 2.f);
				FSlateDrawElement::MakeBox(Out, ++LayerId, Geo.ToPaintGeometry(P, S), Brush, ESlateDrawEffect::None, FLinearColor::White);
			}
		}
	}

	return LayerId;
}

FReply SMultiHandleSlider::OnMouseButtonDown(const FGeometry& Geo, const FPointerEvent& E)
{
	if (E.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();

	const FVector2D Local = Geo.AbsoluteToLocal(E.GetScreenSpacePosition());

	ActiveLane = INDEX_NONE;
	ActiveIndex = INDEX_NONE;

	const float PickRadius = HandleRadius * 1.6f;
	float BestDist = FLT_MAX;

	for (int32 Lane = 0; Lane < LaneValues.Num(); ++Lane)
	{
		const float LaneAxis = (Orientation == Orient_Horizontal) ? LanePosY(Geo, Lane) : LanePosX(Geo, Lane);
		for (int32 i = 0; i < LaneValues[Lane].Num(); ++i)
		{
			const float AxisAlong = Value01ToPixel(Geo, LaneValues[Lane][i]);
			const FVector2D HandleCenter = (Orientation == Orient_Horizontal)
				? FVector2D(AxisAlong, LaneAxis) : FVector2D(LaneAxis, AxisAlong);

			const float Dist = (HandleCenter - Local).Size();
			if (Dist < BestDist && Dist <= PickRadius)
			{
				BestDist = Dist; ActiveLane = Lane; ActiveIndex = i;
			}
		}
	}

	if (ActiveLane == INDEX_NONE)
	{
		const int32 Lane = NearestLane(Geo, Local);
		float V01 = (Orientation == Orient_Horizontal) ? PixelToValue01(Geo, Local.X) : PixelToValue01(Geo, Local.Y);
		V01 = FMath::Clamp(Snap(V01), 0.f, 1.f);

		TArray<float>& Curr = LaneValues[Lane];
		Curr.Add(V01);
		Curr.Sort();

		int32 NewIdx = 0; float Best = FLT_MAX;
		for (int32 i = 0; i < Curr.Num(); ++i)
		{
			const float D = FMath::Abs(Curr[i] - V01);
			if (D < Best) { Best = D; NewIdx = i; }
		}
		ActiveLane = Lane;
		ActiveIndex = NewIdx;

		if (LaneValues.Num() > 0) { Values = LaneValues[0]; }
		if (OnValuesChanged.IsBound())      OnValuesChanged.Execute(Values);
		if (OnLanesChanged.IsBound())       OnLanesChanged.Execute(LaneValues);
	}

	FSlateApplication::Get().SetKeyboardFocus(AsShared());
	return FReply::Handled().CaptureMouse(AsShared());
}

FReply SMultiHandleSlider::OnMouseButtonUp(const FGeometry&, const FPointerEvent& E)
{
	if (E.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();
	ActiveLane = INDEX_NONE;
	ActiveIndex = INDEX_NONE;
	return FReply::Handled().ReleaseMouseCapture();
}

FReply SMultiHandleSlider::OnMouseMove(const FGeometry& Geo, const FPointerEvent& E)
{
	if (!HasMouseCapture() || ActiveLane == INDEX_NONE || ActiveIndex == INDEX_NONE) return FReply::Unhandled();

	const FVector2D Local = Geo.AbsoluteToLocal(E.GetScreenSpacePosition());

	float V01 = (Orientation == Orient_Horizontal) ? PixelToValue01(Geo, Local.X) : PixelToValue01(Geo, Local.Y);
	V01 = FMath::Clamp(Snap(V01), 0.f, 1.f);

	TArray<float>& Curr = LaneValues[ActiveLane];
	const float Eps = 0.001f;

	Curr[ActiveIndex] = V01;
	Curr.Sort();

	int32 NewIdx = 0; float Best = FLT_MAX;
	for (int32 i = 0; i < Curr.Num(); ++i)
	{
		const float D = FMath::Abs(Curr[i] - V01);
		if (D < Best) { Best = D; NewIdx = i; }
	}
	ActiveIndex = NewIdx;

	if (Curr.Num() > 1)
	{
		if (ActiveIndex > 0)               Curr[ActiveIndex] = FMath::Max(Curr[ActiveIndex], Curr[ActiveIndex - 1] + Eps);
		if (ActiveIndex < Curr.Num() - 1)  Curr[ActiveIndex] = FMath::Min(Curr[ActiveIndex], Curr[ActiveIndex + 1] - Eps);
	}

	if (LaneValues.Num() > 0) { Values = LaneValues[0]; }
	if (OnValuesChanged.IsBound()) OnValuesChanged.Execute(Values);
	if (OnLanesChanged.IsBound())  OnLanesChanged.Execute(LaneValues);

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
		const float P = (Orientation == Orient_Horizontal) ? Value01ToPixel(Geo, Curr[i]) - Local.X
			: Value01ToPixel(Geo, Curr[i]) - Local.Y;
		const float Dist = FMath::Abs(P);
		if (Dist < Best && Dist <= HandleRadius * 1.5f) { Best = Dist; BestIdx = i; }
	}
	return BestIdx;
}

float SMultiHandleSlider::Snap(float V) const
{
	if (StepSize > 0.f)
	{
		const float steps = FMath::RoundToFloat(V / StepSize);
		float out = steps * StepSize;

		const float inv = 1.f / StepSize;
		out = FMath::RoundToFloat(out * inv) / inv;

		return FMath::Clamp(out, 0.f, 1.f);
	}
	return V;
}


float SMultiHandleSlider::LanePosY(const FGeometry& Geo, int32 Lane) const
{
	const FVector2D Size = Geo.GetLocalSize();
	const float Top = HandleRadius;
	const float Bottom = Size.Y - HandleRadius;
	const float T = (NumLanes > 1) ? (float)Lane / (float)(NumLanes - 1) : 0.5f;
	return FMath::Lerp(Top, Bottom, T);
}

float SMultiHandleSlider::LanePosX(const FGeometry& Geo, int32 Lane) const
{
	const FVector2D Size = Geo.GetLocalSize();
	const float Left = HandleRadius;
	const float Right = Size.X - HandleRadius;
	const float T = (NumLanes > 1) ? (float)Lane / (float)(NumLanes - 1) : 0.5f;
	return FMath::Lerp(Left, Right, T);
}

int32 SMultiHandleSlider::NearestLane(const FGeometry& Geo, FVector2D Local) const
{
	int32 Best = 0;
	float BestDist = FLT_MAX;
	for (int32 Lane = 0; Lane < NumLanes; ++Lane)
	{
		const float Axis = (Orientation == Orient_Horizontal) ? LanePosY(Geo, Lane) : LanePosX(Geo, Lane);
		const float Dist = FMath::Abs(((Orientation == Orient_Horizontal) ? Local.Y : Local.X) - Axis);
		if (Dist < BestDist) { BestDist = Dist; Best = Lane; }
	}
	return Best;
}


// -------------------------------------------- //
/*
	This is so I can remember stuff for later on :

		clear all keys function call :
			MyMultiHandleSlider->ClearAllKeys();




*/