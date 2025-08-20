// Copyright (C) 2019-2020 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.


#include "VFDetailsCustomization.h"
#include "VFCreator.h"
#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "VFDetailsSave"

TSharedRef<IDetailCustomization> FVFDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FVFDetailsCustomization);
}

void FVFDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CachedDetailBuilder = &DetailBuilder;

	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);
	for (auto Object : CustomizedObjects)
		if (Object.IsValid() && Object->IsA(AVFBound::StaticClass()))
		{
			VFBound = Cast<AVFBound>(Object.Get());
			break;
		}
	if (VFBound == nullptr)
		return;

	int32 NumArrows = VFBound->NumArrows;
	if (NumArrows > 13824)
		ButtonColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
	else if (NumArrows > 8000)
		ButtonColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
	else if (NumArrows > 512)
		ButtonColor = FLinearColor(0.0f, 0.5f, 0.0f, 1.0f);
	else ButtonColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
		IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Vector Field Creator", FText::GetEmpty(), ECategoryPriority::Important);
		Category.AddCustomRow(LOCTEXT("CategorySave", "Save"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("NameSave", "Save to file"))
			//.Font(IDetailLayoutBuilder::GetDetailFont())
			]
			.ValueContent()
			.MinDesiredWidth(200.f)
			[
				SNew(SButton)
				.Text(LOCTEXT("ButtonSave", "Save to 'Saved' project folder"))
				.ToolTipText(LOCTEXT("ToolTipSave", "Save VFCreator.fga in 'Saved' project folder"))
				.OnClicked(this, &FVFDetailsCustomization::OnSaveClicked)
			];
			
		IDetailCategoryBuilder& CategoryVisualisation = DetailBuilder.EditCategory("Visualization");
		//IDetailPropertyRow& SaveRow = CategoryVisualisation.AddProperty("VFUpdate");
		//SaveRow.CustomWidget()
		CategoryVisualisation.AddCustomRow(LOCTEXT("VisualizationRow", "Update"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("VisualizationNameContent", "Show velocity"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MinDesiredWidth(250.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SCheckBox)
				.IsChecked(VFBound->HoldDraw ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged(this, &FVFDetailsCustomization::OnToggleHoldDraw)
				.ToolTipText(FText::Format(LOCTEXT("WarningText", " - check this flag for draw {0} arrows."), NumArrows))
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::FromInt(NumArrows)))
					.ColorAndOpacity(ButtonColor)
				]
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10.f, 0.f)
				.HAlign(HAlign_Left)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(ButtonColor)
				.IsEnabled(VFBound->HoldDraw)
				.Text(LOCTEXT("DrawButtonText", "Draw vectors"))
				.HAlign(HAlign_Center)
				.ToolTipText(LOCTEXT("DrawButtonToolTip", "Update vectors"))
				.OnClicked(this, &FVFDetailsCustomization::OnDrawClicked)
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.f, 0.f)
			.HAlign(HAlign_Left)
			[
				SNew(SButton)
				.Text(LOCTEXT("ClearButtonText", "Clear vectors"))
				.HAlign(HAlign_Center)
				.ToolTipText(LOCTEXT("ClearButtonToolTip", "Remove vectors"))
				.OnClicked(this, &FVFDetailsCustomization::OnClearClicked)
			]
		];
		CategoryVisualisation.AddProperty("SkipGridsNum");
		CategoryVisualisation.AddProperty("bShowVectorsInSphere");
		CategoryVisualisation.AddProperty("bUpdateVectorsInSphereWhenMove");
		CategoryVisualisation.AddProperty("CenterShowSphere");
		CategoryVisualisation.AddProperty("SphereRadius");
		CategoryVisualisation.AddProperty("VectorSize");
		CategoryVisualisation.AddProperty("VectorLengthScale");

	IDetailCategoryBuilder& CategoryCalculate = DetailBuilder.EditCategory("Vector Field Splines");
	CategoryCalculate.AddCustomRow(LOCTEXT("RowCalculate", "Calculate"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CalculateName", "Build velocities"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("CalculateButtonText", "Calculate"))
		.ToolTipText(LOCTEXT("CalculateButtonToolTip", "Need one or more VFSplines"))
		.OnClicked(this, &FVFDetailsCustomization::OnCalculateClicked)
		];
}

FReply FVFDetailsCustomization::OnCalculateClicked()
{
	VFBound->OnCalculateClicked();
	CachedDetailBuilder->ForceRefreshDetails();
	return FReply::Handled();
}

FReply FVFDetailsCustomization::OnSaveClicked()
{
	VFBound->SaveToFile();
	return FReply::Handled();
}

FReply FVFDetailsCustomization::OnDrawClicked()
{
	VFBound->ShowVectors();
	return FReply::Handled();
}

FReply FVFDetailsCustomization::OnClearClicked()
{
	VFBound->RemoveVectors();
	return FReply::Handled();
}

void FVFDetailsCustomization::OnToggleHoldDraw(ECheckBoxState NewCheckedState)
{
	VFBound->HoldDraw = NewCheckedState == ECheckBoxState::Checked;
	CachedDetailBuilder->ForceRefreshDetails();
}

#undef LOCTEXT_NAMESPACE
#endif