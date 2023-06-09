﻿// Copyright 2023 XXiHH, All Rights Reserved.

#include "XXiHHWidget.h"
#include "SlateOptMacros.h"
#include "PropertyCustomizationHelpers.h"
#include "XXiHH.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

const static FMargin XXiHHMargin = FMargin(10.0f, 5.0f, 5.0f, 10.0f);
const static float XXiHHTextWidth = 50.0f;
const static float XXiHHInputWidth = 50.0f;

void SXXiHHWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(XXiHHMargin)
		[
			InitTextureInput()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(XXiHHMargin)
		[
			InitCleanLevelInput()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(XXiHHMargin)
		[
			InitMinDistanceInput()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(XXiHHMargin)
		[
			InitIgnoreKeyWords()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(XXiHHMargin)
		[
			InitSpawnActor()
		]
	];
}

TSharedRef<SWidget> SXXiHHWidget::InitTextureInput()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Select Texture :")))
		]
				
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(SObjectPropertyEntryBox)
			.AllowedClass(UTexture::StaticClass())
			.ObjectPath(this, &SXXiHHWidget::GetTexturePath)
			.OnObjectChanged(this, &SXXiHHWidget::OnTextureChanged)
			.DisplayUseSelected(true)
			.DisplayThumbnail(true)
			.DisplayBrowse(true)
		];
}

FString SXXiHHWidget::GetTexturePath() const
{
	if (GetXXiHHEditorSubsystem()->GetCachedTexture().IsValid())
	{
		const FSoftObjectPath Path(GetXXiHHEditorSubsystem()->GetCachedTexture().Get());
		return Path.GetAssetPathString();
	}
	return TEXT("");
}

void SXXiHHWidget::OnTextureChanged(const FAssetData& InAssetData)
{
	const auto& CachedTexture = Cast<UTexture>(InAssetData.GetAsset());
	if (CachedTexture)
	{
		GetXXiHHEditorSubsystem()->SetCachedTexture(CachedTexture);
		GetXXiHHEditorSubsystem()->SetCachedTexturePackageName(InAssetData.PackageName);
	}
}

TSharedRef<SWidget> SXXiHHWidget::InitCleanLevelInput()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Need to clean up existing Actors?")))
		]
			
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(SCheckBox)
			.IsChecked(this, &SXXiHHWidget::GetIsCleanLevel)
			.OnCheckStateChanged(this, &SXXiHHWidget::SetIsCleanLevel)
		];
}

ECheckBoxState SXXiHHWidget::GetIsCleanLevel() const
{
	if (GetXXiHHEditorSubsystem()->IsCleanLevel())
	{
		return ECheckBoxState::Checked;
	}
	return ECheckBoxState::Unchecked;
}

void SXXiHHWidget::SetIsCleanLevel(ECheckBoxState NewState)
{
	GetXXiHHEditorSubsystem()->SetCleanLevel(NewState == ECheckBoxState::Checked);
}

TSharedRef<SWidget> SXXiHHWidget::InitMinDistanceInput()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Min distance bewteen meshes:")))
		]
					
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(SSpinBox<float>)
			.MinValue(0.0f)
			.MaxValue(1000.0f)
			.Value(this, &SXXiHHWidget::OnGetMinDistance)
			.OnValueChanged(this, &SXXiHHWidget::OnMinDistanceChanged)
		];
}

float SXXiHHWidget::OnGetMinDistance() const
{
	return GetXXiHHEditorSubsystem()->GetMinDistance();
}

void SXXiHHWidget::OnMinDistanceChanged(float NewMinDistance)
{
	GetXXiHHEditorSubsystem()->SetMinDistance(NewMinDistance);
}

TSharedRef<SWidget> SXXiHHWidget::InitIgnoreKeyWords()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Keywords to ignore in the path (separated by commas):")))
		]
					
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(SEditableTextBox)
			.Text(this, &SXXiHHWidget::GetIgnoreKeyWordsText)
			.OnTextCommitted(this, &SXXiHHWidget::SetIgnoreKeyWordsText)
			.MinDesiredWidth(280.f)
		];
}

FText SXXiHHWidget::GetIgnoreKeyWordsText() const
{
	return FText::FromString(GetXXiHHEditorSubsystem()->GetIgnoreKeyWords());
}

void SXXiHHWidget::SetIgnoreKeyWordsText(const FText& InNewText, ETextCommit::Type)
{
	GetXXiHHEditorSubsystem()->SetIgnoreKeyWords(InNewText.ToString());
}

FReply SXXiHHWidget::SpawnActorUseCachedTextureInLevel()
{
	int32 NumSpawnedActor = 0;

	UXXiHHEditorSubsystem* XXiHHEditorSubsystem = GetXXiHHEditorSubsystem();
	
	if (!XXiHHEditorSubsystem->GetCachedTexturePackageName().IsNone())
	{
		TArray<FString> IgnoreKeyWordsArray;
		XXiHHEditorSubsystem->GetIgnoreKeyWords().ParseIntoArray(IgnoreKeyWordsArray, TEXT(","));
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		
		TArray<FName> ReferenceMats;
		AssetRegistryModule.Get().GetReferencers(XXiHHEditorSubsystem->GetCachedTexturePackageName(), ReferenceMats);
		
		TArray<UStaticMesh*> StaticMeshes;
		TArray<USkeletalMesh*> SkeletalMeshes;
		
		FScopedSlowTask MatSlowTask(ReferenceMats.Num(), FText::FromString(TEXT("Find material Referencers...")));
		MatSlowTask.MakeDialog();
		
		for (const auto& ReferenceMat : ReferenceMats)
		{
			MatSlowTask.EnterProgressFrame(1.0f, FText::FromName(ReferenceMat));
			
			TArray<FName> ReferenceMeshes;
			AssetRegistryModule.Get().GetReferencers(ReferenceMat, ReferenceMeshes);
			
			FScopedSlowTask MeshSlowTask(ReferenceMeshes.Num(), FText::FromString(TEXT("Find mesh Referencers...")));
			MeshSlowTask.MakeDialog();
			
			for (const auto& ReferenceMesh : ReferenceMeshes)
			{
				MeshSlowTask.EnterProgressFrame(1.0f, FText::FromName(ReferenceMesh));
				
				FString MeshPackageName = ReferenceMesh.ToString();
				if (IsIgnoreMeshByName(IgnoreKeyWordsArray, MeshPackageName))
				{
					continue;
				}
				MeshPackageName = MeshPackageName + TEXT('.') + FPackageName::GetShortName(MeshPackageName); 
				if (UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *MeshPackageName))
				{
					StaticMeshes.AddUnique(StaticMesh);
				}
				else if (USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPackageName))
				{
					SkeletalMeshes.AddUnique(SkeletalMesh);
				}
			}
		}
		
		TArray<FXXiHHStreamableRenderAsset> StreamableRenderAssets;
		for (const auto& StaticMesh : StaticMeshes)
		{
			StreamableRenderAssets.Add(FXXiHHStreamableRenderAsset(StaticMesh));
		}
		for (const auto& SkeletalMesh :SkeletalMeshes)
		{
			StreamableRenderAssets.Add(FXXiHHStreamableRenderAsset(SkeletalMesh));
		}
		StreamableRenderAssets.Sort();
		
		if (XXiHHEditorSubsystem->IsCleanLevel())
		{
			XXiHHEditorSubsystem->CleanCachedActors();
			XXiHHEditorSubsystem->SetCachedOrigin(FTransform());
		}
		else
		{
			FVector Location = XXiHHEditorSubsystem->GetCachedOrigin().GetLocation();
			if (StreamableRenderAssets.Num() > 0)
			{
				const FVector& LastExtent = StreamableRenderAssets[StreamableRenderAssets.Num() - 1].GetBoundingExtent();
				Location.X += LastExtent.X;
			}
			XXiHHEditorSubsystem->SetCachedOrigin(FTransform(Location));
		}
		
		FScopedSlowTask SpawnSlowTask(StreamableRenderAssets.Num(), FText::FromString(TEXT("Spawn Actors...")));
		SpawnSlowTask.MakeDialog();
		
		for (int32 Index = 0; Index < StreamableRenderAssets.Num(); ++ Index)
		{
			const FXXiHHStreamableRenderAsset& StreamableRenderAsset = StreamableRenderAssets[Index];
			
			SpawnSlowTask.EnterProgressFrame(1.0f, FText::FromString(StreamableRenderAsset.StreamableRenderAsset->GetName()));
			
			AActor* Actor = StreamableRenderAsset.SpawnActorByTransform(XXiHHEditorSubsystem->GetCachedOrigin());
			NumSpawnedActor ++;
			XXiHHEditorSubsystem->AddCachedActor(Actor);
			
			const FVector& CurrExtent = StreamableRenderAsset.GetBoundingExtent();
			FVector Location = XXiHHEditorSubsystem->GetCachedOrigin().GetLocation();
			if (Index < StreamableRenderAssets.Num() - 1)
			{
				const FVector& NextExtent = StreamableRenderAssets[Index + 1].GetBoundingExtent();
				Location.Y += FMath::Max(CurrExtent.Y, XXiHHEditorSubsystem->GetMinDistance());
				Location.Y += FMath::Max(NextExtent.Y, XXiHHEditorSubsystem->GetMinDistance());
			}
			else
			{
				Location.X += FMath::Max(CurrExtent.X, XXiHHEditorSubsystem->GetMinDistance());
				Location.Y = 0.0f;
			}
			XXiHHEditorSubsystem->SetCachedOrigin(FTransform(Location));
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok,
		NumSpawnedActor > 0 ?
		FText::Format(FTextFormat::FromString("{0} StaticMeshActors/SkeletalMeshActors spawned!"), NumSpawnedActor) :
		FText::FromString("No mesh referencer found!"));
	
	return FReply::Handled();
}

TSharedRef<SWidget> SXXiHHWidget::InitSpawnActor()
{
	return
		SNew(SHorizontalBox)
		/*
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2, 2, 2, 2)
		[
			SNew(SBox)
			.WidthOverride(150)
			.HeightOverride(30)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("")))
			]
		]
		*/

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(XXiHHMargin)
		[
			SNew(SBox)
			.WidthOverride(150)
			.HeightOverride(30)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.OnClicked(this, &SXXiHHWidget::SpawnActorUseCachedTextureInLevel)
				.Text(FText::FromString(TEXT("Spawn!")))
			]
		];
}

bool SXXiHHWidget::IsIgnoreMeshByName(const TArray<FString>& IgnoreKeyWordsArray, const FString& PackageName) const
{
	for (const auto& IgnoreKeyWord : IgnoreKeyWordsArray)
	{
		if (PackageName.Contains(IgnoreKeyWord))
		{
			return true;
		}
	}
	return false;
}

UXXiHHEditorSubsystem* SXXiHHWidget::GetXXiHHEditorSubsystem() const
{
	return GEditor->GetEditorSubsystem<UXXiHHEditorSubsystem>();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
