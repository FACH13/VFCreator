// Copyright (C) 2019-2020 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.

#pragma once

#if WITH_EDITOR

#include "Editor/DetailCustomizations/Public/DetailCustomizations.h"
#include "Editor/PropertyEditor/Public/IDetailCustomization.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "VFBound.h"

class FVFDetailsCustomization : public IDetailCustomization
{
public:
	
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	IDetailLayoutBuilder* CachedDetailBuilder;

	AVFBound* VFBound;
	FLinearColor ButtonColor;

	FReply OnCalculateClicked();
	FReply OnSaveClicked();
	FReply OnDrawClicked();
	FReply OnClearClicked();
	void OnToggleHoldDraw(ECheckBoxState NewCheckedState);
};


#endif