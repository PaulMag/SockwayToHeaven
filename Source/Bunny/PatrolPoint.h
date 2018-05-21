// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPoint.generated.h"

UCLASS()
class BUNNY_API APatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APatrolPoint();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		int zone = 0;  // Specify patrol zones in the PatrolPoint instances.
	
	virtual void Tick(float DeltaTime) override;
};
