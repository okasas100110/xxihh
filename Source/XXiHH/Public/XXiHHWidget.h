// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"


struct FXXiHHStreamableRenderAsset
{
	UStreamableRenderAsset* StreamableRenderAsset;

	FXXiHHStreamableRenderAsset (UStreamableRenderAsset* InStreamableRenderAsset)
		: StreamableRenderAsset(InStreamableRenderAsset)
	{}

	AActor* SpawnActorByTransform(const FTransform& InTransform) const
	{
		if (const auto& StaticMesh = Cast<UStaticMesh>(StreamableRenderAsset))
		{
			const auto& StaticMeshActor = GWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), InTransform);
			StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
			StaticMeshActor->SetActorLabel(StaticMesh->GetName());
			return StaticMeshActor;
		}
		else if (const auto& SkeletalMesh = Cast<USkeletalMesh>(StreamableRenderAsset))
		{
			const auto& SkeletalMeshActor = GWorld->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), InTransform);
			SkeletalMeshActor->GetSkeletalMeshComponent()->SetSkeletalMesh(SkeletalMesh);
			SkeletalMeshActor->SetActorLabel(SkeletalMesh->GetName());
			return SkeletalMeshActor;
		}
		return nullptr;
	}

	FVector GetBoundingExtent() const
	{
		FVector Extent(ForceInit);
		if (const auto& StaticMesh = Cast<UStaticMesh>(StreamableRenderAsset))
		{
			Extent = StaticMesh->GetBoundingBox().GetExtent();
		}
		else if (const auto& SkeletalMesh = Cast<USkeletalMesh>(StreamableRenderAsset))
		{
			const FBoxSphereBounds& BoxSphereBounds = SkeletalMesh->GetBounds();
			Extent = BoxSphereBounds.GetBox().GetExtent();
		}
		return Extent;
	}

	float GetBoundingSize () const
	{
		return GetBoundingExtent().Size();
	}

	bool operator < (const FXXiHHStreamableRenderAsset& Others) const
	{
		return GetBoundingSize() < Others.GetBoundingSize();
	}
};

class XXIHH_API SXXiHHWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SXXiHHWidget)
	{
	}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<SWidget> InitTextureInput();
	FString GetTexturePath	() const;
	void OnTextureChanged	(const FAssetData& InAssetData);

	TSharedRef<SWidget> InitCleanLevelInput();
	ECheckBoxState GetIsCleanLevel() const { return bCleanLevel; }
	void SetIsCleanLevel(ECheckBoxState NewState) { bCleanLevel = NewState; }

	TSharedRef<SWidget> InitIgnoreKeyWords();
	FText GetIgnoreKeyWordsText() const;
	void SetIgnoreKeyWordsText(const FText& InNewText, ETextCommit::Type);

	FReply SpawnActorUseCachedTextureInLevel();

	TSharedRef<SWidget> InitSpawnActor();

	bool IsIgnoreMeshByName (const TArray<FString>& IgnoreKeyWords, const FString& PackageName) const;

protected:
	TWeakObjectPtr<UTexture> CachedTexture;

	FName CachedTexturePackageName;

	ECheckBoxState bCleanLevel = ECheckBoxState::Checked;

	FString IgnoreKeyWords;

	TArray<TWeakObjectPtr<AActor>> CachedActors;

	FTransform CachedOrigin;
};
