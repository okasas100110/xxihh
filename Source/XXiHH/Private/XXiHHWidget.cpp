// Fill out your copyright notice in the Description page of Project Settings.

#include "XXiHHWidget.h"
#include "SlateOptMacros.h"
#include "PropertyCustomizationHelpers.h"
#include "XXiHH.h"
#include "AssetRegistry/AssetRegistryModule.h"

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
	if (CachedTexture.IsValid())
	{
		const FSoftObjectPath Path(CachedTexture.Get());
		return Path.GetAssetPathString();
	}
	return TEXT("");
}

void SXXiHHWidget::OnTextureChanged(const FAssetData& InAssetData)
{
	CachedTexture = Cast<UTexture>(InAssetData.GetAsset());
	if (CachedTexture.IsValid())
	{
		CachedTexturePackageName = InAssetData.PackageName;
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
			.MinDesiredWidth(300.f)
		];
}

FText SXXiHHWidget::GetIgnoreKeyWordsText() const
{
	return FText::FromString(IgnoreKeyWords);
}

void SXXiHHWidget::SetIgnoreKeyWordsText(const FText& InNewText, ETextCommit::Type)
{
	IgnoreKeyWords = InNewText.ToString();
}

FReply SXXiHHWidget::SpawnActorUseCachedTextureInLevel()
{
	if (!CachedTexturePackageName.IsNone())
	{
		TArray<FString> IgnoreKeyWordsArray;
		IgnoreKeyWords.ParseIntoArray(IgnoreKeyWordsArray, TEXT(","));
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		TArray<FName> ReferenceMats;
		AssetRegistryModule.Get().GetReferencers(CachedTexturePackageName, ReferenceMats);
		TArray<UStaticMesh*> StaticMeshes;
		TArray<USkeletalMesh*> SkeletalMeshes;
		for (const auto& ReferenceMat : ReferenceMats)
		{
			TArray<FName> ReferenceMeshes;
			AssetRegistryModule.Get().GetReferencers(ReferenceMat, ReferenceMeshes);
			for (const auto& ReferenceMesh : ReferenceMeshes)
			{
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
		if (GetIsCleanLevel() == ECheckBoxState::Checked)
		{
			for (const auto& CachedActor : CachedActors)
			{
				if (CachedActor.IsValid())
				{
					CachedActor->Destroy();
				}
			}
			CachedActors.Empty();
			CachedOrigin = FTransform();
		}
		else
		{
			FVector Location = CachedOrigin.GetLocation();
			if (StreamableRenderAssets.Num() > 0)
			{
				const FVector& LastExtent = StreamableRenderAssets[StreamableRenderAssets.Num() - 1].GetBoundingExtent();
				Location.X += LastExtent.X;
			}
			CachedOrigin.SetLocation(Location);
		}
		for (int32 Index = 0; Index < StreamableRenderAssets.Num(); ++ Index)
		{
			const FXXiHHStreamableRenderAsset& StreamableRenderAsset = StreamableRenderAssets[Index]; 
			AActor* Actor = StreamableRenderAsset.SpawnActorByTransform(CachedOrigin);
			CachedActors.Add(Actor);
			const FVector& CurrExtent = StreamableRenderAsset.GetBoundingExtent();
			FVector Location = CachedOrigin.GetLocation();
			if (Index < StreamableRenderAssets.Num() - 1)
			{
				const FVector& NextExtent = StreamableRenderAssets[Index + 1].GetBoundingExtent();
				Location.Y += FMath::Max(CurrExtent.Y, 50.0f);
				Location.Y += FMath::Max(NextExtent.Y, 50.0f);
			}
			else
			{
				Location.X += FMath::Max(CurrExtent.X, 50.0f);
				Location.Y = 0.0f;
			}
			CachedOrigin.SetLocation(Location);
		}
	}
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
			.HeightOverride(20)
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

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
