// Copyright Epic Games, Inc. All Rights Reserved.

#include "XXiHH.h"
#include "XXiHHStyle.h"
#include "XXiHHCommands.h"
#include "Misc\MessageDialog.h"
#include "ToolMenus.h"
#include "Editor\WorkspaceMenuStructure\Public\WorkspaceMenuStructure.h"
#include "Editor\WorkspaceMenuStructure\Public\WorkspaceMenuStructureModule.h"
#include "XXiHHWidget.h"

DEFINE_LOG_CATEGORY(LOGXXiHH);

static const FName XXiHHTabName("XXiHH");
#define LOCTEXT_NAMESPACE "FXXiHHModule"

void FXXiHHModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FXXiHHStyle::Initialize();
	FXXiHHStyle::ReloadTextures();

	FXXiHHCommands::Register();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(XXiHHTabName, FOnSpawnTab::CreateLambda(
		[](const FSpawnTabArgs& SpawnTabArgs)
		{
			return SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				[
					SNew(SXXiHHWidget)
				];
		}))
		.SetDisplayName(LOCTEXT("XXXiHHDisplayName", "XXiHH Mesh Display tool"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FXXiHHCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FXXiHHModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FXXiHHModule::RegisterMenus));
}

void FXXiHHModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FXXiHHStyle::Shutdown();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(XXiHHTabName);

	FXXiHHCommands::Unregister();
}

void FXXiHHModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here

	FGlobalTabmanager::Get()->TryInvokeTab(XXiHHTabName);
}

void FXXiHHModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FXXiHHCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FXXiHHCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXXiHHModule, XXiHH)