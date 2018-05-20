// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AreaTrigger.h"
#include "CheckpointBase.generated.h"

/**
 * 
 */
UCLASS()
class BUNNY_API ACheckpointBase : public AAreaTrigger
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		FString effect = "";
	
};
