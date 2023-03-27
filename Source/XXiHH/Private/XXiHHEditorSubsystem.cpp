// Copyright 2023 XXiHH, All Rights Reserved.

#include "XXiHHEditorSubsystem.h"

TWeakObjectPtr<UTexture> UXXiHHEditorSubsystem::GetCachedTexture() const
{
	return CachedTexture;
}

void UXXiHHEditorSubsystem::SetCachedTexture(const TWeakObjectPtr<UTexture>& NewCachedTexture)
{
	this->CachedTexture = NewCachedTexture;
}

FName UXXiHHEditorSubsystem::GetCachedTexturePackageName() const
{
	return CachedTexturePackageName;
}

void UXXiHHEditorSubsystem::SetCachedTexturePackageName(const FName& NewCachedTexturePackageName)
{
	this->CachedTexturePackageName = NewCachedTexturePackageName;
}

float UXXiHHEditorSubsystem::GetMinDistance() const
{
	return MinDistance;
}

void UXXiHHEditorSubsystem::SetMinDistance(const float NewMinDistance)
{
	this->MinDistance = NewMinDistance;
}

bool UXXiHHEditorSubsystem::IsCleanLevel() const
{
	return bCleanLevel;
}

void UXXiHHEditorSubsystem::SetCleanLevel(const bool NewCleanLevel)
{
	this->bCleanLevel = NewCleanLevel;
}

FString UXXiHHEditorSubsystem::GetIgnoreKeyWords() const
{
	return IgnoreKeyWords;
}

void UXXiHHEditorSubsystem::SetIgnoreKeyWords(const FString& NewIgnoreKeyWords)
{
	this->IgnoreKeyWords = NewIgnoreKeyWords;
}

TArray<TWeakObjectPtr<AActor>> UXXiHHEditorSubsystem::GetCachedActors() const
{
	return CachedActors;
}

void UXXiHHEditorSubsystem::AddCachedActor(AActor* NewCachedActor)
{
	this->CachedActors.Add(NewCachedActor);
}

void UXXiHHEditorSubsystem::CleanCachedActors()
{
	for (const auto& CachedActor : CachedActors)
	{
		if (CachedActor.IsValid())
		{
			CachedActor->Destroy();
		}
	}
	this->CachedActors.Empty();
}

FTransform UXXiHHEditorSubsystem::GetCachedOrigin() const
{
	return CachedOrigin;
}

void UXXiHHEditorSubsystem::SetCachedOrigin(const FTransform& NewCachedOrigin)
{
	this->CachedOrigin = NewCachedOrigin;
}
