// Copyright (C) 2019-2020 Molchanov Kirill FACHMolchanov@gmail.com. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FVFCreatorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;

	virtual bool IsGameModule() const override
	{
		return false;
	}
};