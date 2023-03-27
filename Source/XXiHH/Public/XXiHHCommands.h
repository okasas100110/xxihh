// Copyright 2023 XXiHH, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "XXiHHStyle.h"

class FXXiHHCommands : public TCommands<FXXiHHCommands>
{
public:

	FXXiHHCommands()
		: TCommands<FXXiHHCommands>(TEXT("XXiHH"), NSLOCTEXT("Contexts", "XXiHH", "XXiHH Plugin"), NAME_None, FXXiHHStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
