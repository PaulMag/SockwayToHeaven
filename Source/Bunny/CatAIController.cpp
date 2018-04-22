// Fill out your copyright notice in the Description page of Project Settings.

#include "BunnyCharacter.h"
#include "CatAIController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "TimerManager.h"


ACatAIController::ACatAIController()
{
}

void ACatAIController::BeginPlay()
{
	Super::BeginPlay();
	playerPawn = Cast<ABunnyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	pawn = Cast<AEnemyCat>(GetPawn());
	pawn->controller = this;
	paceToRandomPoint();
}

void ACatAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACatAIController::setAlertMode(int mode)
{
	alertMode = mode;
	if (alertMode == chasing)
	{
		GetWorldTimerManager().ClearTimer(moveTimerHandle);
	}
	takeAction();
}

void ACatAIController::paceToRandomPoint()
{
	float direction = FMath::RandRange(0.f, 2*PI);
	float distance = FMath::RandRange(100.f, 300.f);
	target.X = cos(direction) * distance;
	target.Y = sin(direction) * distance;
	target += pawn->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move started to (%f, %f)"), target.X, target.Y);
	MoveToLocation(target, 5);
}

void ACatAIController::chasePlayer()
{
	MoveToActor(playerPawn, 0);
}

void ACatAIController::takeAction()
{
	StopMovement();
	GetWorldTimerManager().ClearTimer(moveTimerHandle);

	if (alertMode == chasing && pawn->isInAttackRange())
	{
		pawn->attackBegin();
	}
	else if (alertMode == chasing)
	{
		chasePlayer();
	}
	else
	{
		paceToRandomPoint();
	}
}

void ACatAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move completed"));
	if (alertMode == chasing)
	{
		/* Default to attacking when done chasing.
		 * This happens both if chase stops because player was reached or if chase 
		 * stopped because no further path to the player.
		 * If the cat knows where the bunny is but can't reach it
		 * he might as well stand and attack in the air and try to be intimidating.
		*/
		pawn->attackBegin();
	}
	else
	{
		GetWorldTimerManager().SetTimer(moveTimerHandle, this, &ACatAIController::takeAction, 2.0, false);
	}
}
