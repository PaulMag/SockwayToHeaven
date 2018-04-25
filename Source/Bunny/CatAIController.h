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
	enum { calm = 0, spooked = 1 };

	AEnemyCat* pawn;
	
	virtual void BeginPlay() override;

	ABunnyCharacter* playerPawn;
	FVector target;
	FVector spookLocation;
	FTimerHandle moveTimerHandle;
	int alertMode = idle;
	int spookMode = calm;
	void wait();
	void paceToRandomPoint();
	void paceToPlayer();
	void chasePlayer();
	void flee();

public:
	ACatAIController();
	virtual void Tick(float DeltaTime) override;
	void takeAction();
	void setAlertMode(int mode);
	void setSpookMode(int mode);
	void setSpookMode(int mode, FVector location);
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
