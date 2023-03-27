// Copyright 2023 XXiHH, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XXiHHEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class XXIHH_API UXXiHHEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	
	TWeakObjectPtr<UTexture> GetCachedTexture() const;
	void SetCachedTexture(const TWeakObjectPtr<UTexture>& NewCachedTexture);
	
	FName GetCachedTexturePackageName() const;
	void SetCachedTexturePackageName(const FName& NewCachedTexturePackageName);
	
	float GetMinDistance() const;
	void SetMinDistance(const float NewMinDistance);
	
	bool IsCleanLevel() const;
	void SetCleanLevel(const bool NewCleanLevel);
	
	FString GetIgnoreKeyWords() const;
	void SetIgnoreKeyWords(const FString& NewIgnoreKeyWords);
	
	TArray<TWeakObjectPtr<AActor>> GetCachedActors() const;
	void AddCachedActor(AActor* NewCachedActors);
	void CleanCachedActors();
	
	FTransform GetCachedOrigin() const;
	void SetCachedOrigin(const FTransform& NewCachedOrigin);

private:
	
	UPROPERTY()
	TWeakObjectPtr<UTexture> CachedTexture;
	UPROPERTY()
	FName CachedTexturePackageName;
	UPROPERTY()
	float MinDistance = 50.0f;
	UPROPERTY()
	bool bCleanLevel = true;
	UPROPERTY()
	FString IgnoreKeyWords;
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> CachedActors;
	UPROPERTY()
	FTransform CachedOrigin;
};
