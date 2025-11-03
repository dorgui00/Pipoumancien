#include "Tools/Slider/MultiHandleSlider.h"
#include "Tools/Slider/SMultiHandleSlider.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

void UMultiHandleSlider::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MySlateWidget.Reset();
}

TSharedRef<SWidget> UMultiHandleSlider::RebuildWidget()
{
	if (Lanes.Num() != NumLanes)
	{
		Lanes.SetNum(NumLanes);
	}

	TArray<TArray<float>> LaneValues;
	BuildSlateLaneArray(LaneValues);

	SAssignNew(MySlateWidget, SMultiHandleSlider)
		.Orientation(Orientation)
		.BarThickness(BarThickness)
		.HandleRadius(HandleRadius)
		.StepSize(StepSize)
		.LaneValues(LaneValues)
		.OnValuesChanged(FOnMultiSliderChanged::CreateUObject(this, &UMultiHandleSlider::Slate_OnValuesChanged))
		.OnLanesChanged(FOnMultiSliderLanesChanged::CreateUObject(this, &UMultiHandleSlider::Slate_OnLanesChanged));

	return MySlateWidget.ToSharedRef();
}

void UMultiHandleSlider::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (Lanes.Num() != NumLanes)
	{
		Lanes.SetNum(NumLanes);
	}
	for (FMultiSliderLane& Lane : Lanes)
	{
		for (float& V : Lane.Values01) V = FMath::Clamp(V, 0.f, 1.f);
		Lane.Values01.Sort();
		Lane.Keys.SetNum(Lane.Values01.Num());
	}

	if (MySlateWidget.IsValid())
	{
		TArray<TArray<float>> LaneValues;
		BuildSlateLaneArray(LaneValues);
		MySlateWidget->SetLaneValues(LaneValues);

		MySlateWidget->SetHandleRadius(HandleRadius);
		MySlateWidget->SetBarThickness(BarThickness);
		MySlateWidget->SetOrientation(Orientation);

		MySlateWidget->Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}


	auto SnapToStep = [this](float x)
		{
			if (StepSize > 0.f)
			{
				const float s = FMath::RoundToFloat(x / StepSize) * StepSize;
				const float inv = 1.f / StepSize;
				return FMath::RoundToFloat(s * inv) / inv;
			}
			return x;
		};

	for (FMultiSliderLane& Lane : Lanes)
	{
		for (float& V : Lane.Values01)
		{
			V = FMath::Clamp(SnapToStep(V), 0.f, 1.f);
		}
		Lane.Values01.Sort();
		Lane.Keys.SetNum(Lane.Values01.Num());
	}

}
#if WITH_EDITOR

#if WITH_EDITOR
void UMultiHandleSlider::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (Lanes.Num() != NumLanes)
	{
		Lanes.SetNum(NumLanes);
	}

	for (FMultiSliderLane& Lane : Lanes)
	{
		for (float& V : Lane.Values01) { V = FMath::Clamp(V, 0.f, 1.f); }
		Lane.Values01.Sort();

		if (Lane.Keys.Num() < Lane.Values01.Num())
		{
			Lane.Keys.AddDefaulted(Lane.Values01.Num() - Lane.Keys.Num());
		}
		else if (Lane.Keys.Num() > Lane.Values01.Num())
		{
			Lane.Keys.SetNum(Lane.Values01.Num());
		}

		auto SnapToStep = [this](float x)
			{
				if (StepSize > 0.f)
				{
					const float s = FMath::RoundToFloat(x / StepSize) * StepSize;
					const float inv = 1.f / StepSize;
					return FMath::RoundToFloat(s * inv) / inv;
				}
				return x;
			};
	}

	if (MySlateWidget.IsValid())
	{
		TArray<TArray<float>> LaneValues;
		BuildSlateLaneArray(LaneValues);
		MySlateWidget->SetLaneValues(LaneValues);
		MySlateWidget->Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}
#endif

void UMultiHandleSlider::GetSortedHandles(TArray<FMultiSliderHandleSorted>& OutSorted) const
{
	OutSorted.Reset();

	for (int32 LaneIdx = 0; LaneIdx < Lanes.Num(); ++LaneIdx)
	{
		const FMultiSliderLane& Lane = Lanes[LaneIdx];
		const int32 Count = Lane.Values01.Num();

		for (int32 i = 0; i < Count; ++i)
		{
			FMultiSliderHandleSorted H;
			H.Distance01 = Lane.Values01[i];
			H.Lane = LaneIdx;
			H.InputKey = (i < Lane.Keys.Num()) ? Lane.Keys[i] : NAME_None;
			OutSorted.Add(H);
		}
	}

	OutSorted.Sort([](const FMultiSliderHandleSorted& A, const FMultiSliderHandleSorted& B)
		{
			if (A.Distance01 != B.Distance01) return A.Distance01 < B.Distance01;
			if (A.Lane != B.Lane)             return A.Lane < B.Lane;
			return A.InputKey.LexicalLess(B.InputKey);
		});
}

#endif

void UMultiHandleSlider::Slate_OnValuesChanged(const TArray<float>& NewLane0)
{
	if (Lanes.Num() == 0)
	{
		Lanes.SetNum(NumLanes);
	}
	if (Lanes.Num() > 0)
	{
		Lanes[0].Values01 = NewLane0;
	}
	OnValuesChanged.Broadcast(NewLane0);
}

void UMultiHandleSlider::Slate_OnLanesChanged(const TArray<TArray<float>>& NewAll)
{
	ApplySlateLaneArray(NewAll);
	OnLanesChanged.Broadcast(Lanes);
}

void UMultiHandleSlider::BuildSlateLaneArray(TArray<TArray<float>>& Out) const
{
	Out.Reset();
	Out.Reserve(Lanes.Num());
	for (const FMultiSliderLane& L : Lanes)
	{
		Out.Add(L.Values01);
	}
}

void UMultiHandleSlider::ApplySlateLaneArray(const TArray<TArray<float>>& In)
{
	const int32 N = In.Num();
	NumLanes = N;
	Lanes.SetNum(N);
	for (int32 i = 0; i < N; ++i)
	{
		Lanes[i].Values01 = In[i];
		Lanes[i].Values01.Sort();
	}
}


//blueprints

void UMultiHandleSlider::ClearAllKeys()
{
	if (Lanes.Num() != NumLanes)
		Lanes.SetNum(NumLanes);

	for (FMultiSliderLane& Lane : Lanes)
	{
		Lane.Values01.Reset();
		Lane.Keys.Reset();
	}

#if WITH_EDITOR
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		Modify();
	}
#endif

	PushToSlate();
	OnLanesChanged.Broadcast(Lanes);
	OnValuesChanged.Broadcast({});
}

float UMultiHandleSlider::SnapToStep(float X) const
{
	if (StepSize > 0.f)
	{
		const float steps = FMath::RoundToFloat(X / StepSize);
		float out = steps * StepSize;
		const float inv = 1.f / StepSize;
		out = FMath::RoundToFloat(out * inv) / inv;
		return FMath::Clamp(out, 0.f, 1.f);
	}
	return FMath::Clamp(X, 0.f, 1.f);
}

void UMultiHandleSlider::SortLaneKeepKeys(FMultiSliderLane& Lane)
{
	struct FPair { float V; FName K; };
	TArray<FPair> Pairs;
	Pairs.Reserve(Lane.Values01.Num());

	if (Lane.Keys.Num() < Lane.Values01.Num())
	{
		Lane.Keys.AddDefaulted(Lane.Values01.Num() - Lane.Keys.Num());
	}
	else if (Lane.Keys.Num() > Lane.Values01.Num())
	{
		Lane.Keys.SetNum(Lane.Values01.Num());
	}

	for (int32 i = 0; i < Lane.Values01.Num(); ++i)
	{
		Pairs.Add({ Lane.Values01[i], Lane.Keys[i] });
	}

	Pairs.Sort([](const FPair& A, const FPair& B)
		{
			return A.V < B.V;
		});

	for (int32 i = 0; i < Pairs.Num(); ++i)
	{
		Lane.Values01[i] = Pairs[i].V;
		Lane.Keys[i] = Pairs[i].K;
	}
}

void UMultiHandleSlider::PushToSlate()
{
	if (MySlateWidget.IsValid())
	{
		TArray<TArray<float>> LaneValues;
		LaneValues.Reserve(Lanes.Num());
		for (const FMultiSliderLane& L : Lanes)
		{
			LaneValues.Add(L.Values01);
		}
		MySlateWidget->SetLaneValues(LaneValues);
		MySlateWidget->Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

TArray<FMultiSliderLane> UMultiHandleSlider::GetAllLanes() const
{
	return Lanes;
}

TArray<FMultiSliderHandleSorted> UMultiHandleSlider::GetAllHandlesSorted() const
{
	TArray<FMultiSliderHandleSorted> Out;
	for (int32 LaneIdx = 0; LaneIdx < Lanes.Num(); ++LaneIdx)
	{
		const FMultiSliderLane& Lane = Lanes[LaneIdx];
		const int32 Count = Lane.Values01.Num();
		for (int32 i = 0; i < Count; ++i)
		{
			FMultiSliderHandleSorted H;
			H.Distance01 = Lane.Values01[i];
			H.Lane = LaneIdx;
			H.InputKey = (i < Lane.Keys.Num()) ? Lane.Keys[i] : NAME_None;
			Out.Add(H);
		}
	}

	Out.Sort([](const FMultiSliderHandleSorted& A, const FMultiSliderHandleSorted& B)
		{
			if (A.Distance01 != B.Distance01) return A.Distance01 < B.Distance01;
			if (A.Lane != B.Lane)             return A.Lane < B.Lane;
			return A.InputKey.LexicalLess(B.InputKey);
		});
	return Out;
}

void UMultiHandleSlider::SetAllLanes(const TArray<FMultiSliderLane>& NewLanes)
{
	Lanes = NewLanes;

	NumLanes = Lanes.Num();

	for (FMultiSliderLane& Lane : Lanes)
	{
		for (float& V : Lane.Values01) { V = SnapToStep(V); }
		SortLaneKeepKeys(Lane);
	}

	PushToSlate();
	OnLanesChanged.Broadcast(Lanes);
	if (Lanes.Num() > 0) { OnValuesChanged.Broadcast(Lanes[0].Values01); }
}

void UMultiHandleSlider::SetKeysForLane(int32 LaneIndex, const TArray<FName>& NewKeys)
{
	if (!Lanes.IsValidIndex(LaneIndex)) return;

	FMultiSliderLane& Lane = Lanes[LaneIndex];

	Lane.Keys = NewKeys;
	if (Lane.Keys.Num() < Lane.Values01.Num())
	{
		Lane.Keys.AddDefaulted(Lane.Values01.Num() - Lane.Keys.Num());
	}
	else if (Lane.Keys.Num() > Lane.Values01.Num())
	{
		Lane.Keys.SetNum(Lane.Values01.Num());
	}

	OnLanesChanged.Broadcast(Lanes);
}

void UMultiHandleSlider::SetKeyAt(int32 LaneIndex, int32 HandleIndex, FName NewKey)
{
	if (!Lanes.IsValidIndex(LaneIndex)) return;

	FMultiSliderLane& Lane = Lanes[LaneIndex];

	if (Lane.Keys.Num() < Lane.Values01.Num())
	{
		Lane.Keys.AddDefaulted(Lane.Values01.Num() - Lane.Keys.Num());
	}

	if (Lane.Values01.IsValidIndex(HandleIndex))
	{
		Lane.Keys[HandleIndex] = NewKey;
		OnLanesChanged.Broadcast(Lanes);
	}
}