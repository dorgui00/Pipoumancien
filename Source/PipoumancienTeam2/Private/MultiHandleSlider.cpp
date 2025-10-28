
#include "MultiHandleSlider.h"
#include "SMultiHandleSlider.h"
#include "Widgets/DeclarativeSyntaxSupport.h"


void UMultiHandleSlider::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MySlateWidget.Reset();
}

TSharedRef<SWidget> UMultiHandleSlider::RebuildWidget()
{
	SAssignNew(MySlateWidget, SMultiHandleSlider)
		.Orientation(Orientation)
		.BarThickness(BarThickness)
		.HandleRadius(HandleRadius)
		.StepSize(StepSize)
		.Values01(Values01)
		.OnValuesChanged(FOnMultiSliderChanged::CreateUObject(this, &UMultiHandleSlider::Slate_OnValuesChanged));

	return MySlateWidget.ToSharedRef();
}

void UMultiHandleSlider::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	Values01.Sort();
	if (MySlateWidget.IsValid())
	{
		MySlateWidget->Construct(
			SMultiHandleSlider::FArguments()
			.Orientation(Orientation)
			.BarThickness(BarThickness)
			.HandleRadius(HandleRadius)
			.StepSize(StepSize)
			.Values01(Values01)
			.OnValuesChanged(FOnMultiSliderChanged::CreateUObject(this, &UMultiHandleSlider::Slate_OnValuesChanged))
		);
	}
}

void UMultiHandleSlider::Slate_OnValuesChanged(const TArray<float>& NewValues)
{
	Values01 = NewValues;
	OnValuesChanged.Broadcast(Values01);
}
