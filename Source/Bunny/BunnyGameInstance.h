// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BunnyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BUNNY_API UBunnyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UBunnyGameInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
		bool bCanGlide = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
		bool bCanClimb = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
		bool bCanScare = false;
	
	
};
