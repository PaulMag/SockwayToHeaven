// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BunnyCharacter.h"
#include "EnemyCat.h"

#include "CoreMinimal.h"
#include "AIController.h"
#include "CatAIController.generated.h"


UCLASS()
class BUNNY_API ACatAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	enum { idle = 0, suspicious = 1, chasing = 2 };

	AEnemyCat* pawn;
	
	virtual void BeginPlay() override;

	ABunnyCharacter* playerPawn;
	FVector target;
	FTimerHandle moveTimerHandle;
	int alertMode = idle;
	bool bAttackMode = false;
	void paceToRandomPoint();
	void chasePlayer();

public:
	ACatAIController();
	virtual void Tick(float DeltaTime) override;
	void takeAction();
	void setAlertMode(int mode);
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
