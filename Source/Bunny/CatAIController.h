// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyCat.h"

#include "CoreMinimal.h"
#include "AIController.h"
#include "CatAIController.generated.h"

/**
 * 
 */
UCLASS()
class BUNNY_API ACatAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	AEnemyCat* pawn;
	
	virtual void BeginPlay() override;

	FVector target;
	void paceToRandomPoint();
	FTimerHandle moveTimerHandle;

public:
	ACatAIController();
	virtual void Tick(float DeltaTime) override;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
