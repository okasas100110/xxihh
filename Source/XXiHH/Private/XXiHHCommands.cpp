// Copyright 2023 XXiHH, All Rights Reserved.

#include "XXiHHCommands.h"

#define LOCTEXT_NAMESPACE "FXXiHHModule"

void FXXiHHCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "XXiHH", "Execute XXiHH action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
