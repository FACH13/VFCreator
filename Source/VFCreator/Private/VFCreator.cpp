// Copyright (C) 2019-2020 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.
#pragma once

#include "VFCreator.h"

#if WITH_EDITOR
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "VFDetailsCustomization.h"
#endif

//IMPLEMENT_PRIMARY_GAME_MODULE(FVFCreatorModule, VFCreator, "VFCreator");

void FVFCreatorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITORONLY_DATA

	//UE_LOG(LogTemp, Warning, TEXT("VF Creator Started WE!"));
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("VFBound", FOnGetDetailCustomizationInstance::CreateStatic(&FVFDetailsCustomization::MakeInstance));

#endif
}

IMPLEMENT_MODULE(FVFCreatorModule, VFCreator)