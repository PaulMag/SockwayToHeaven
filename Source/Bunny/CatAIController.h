// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
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
	
	UAIPerceptionComponent* perceptionComponent;
	UAISenseConfig_Sight* sightConfig;

	void OnMoveCompleted(uint32 RequestID, EPathFollowingResult::Type Result);

public:
	ACatAIController();
	virtual void Tick(float DeltaTime) override;
};
