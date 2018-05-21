// Fill out your copyright notice in the Description page of Project Settings.

#include "CatAIController.h"
#include "BunnyCharacter.h"
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
	
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APatrolPoint::StaticClass(), foundActors);
	APatrolPoint* patrolPoint;
	for (int i=0; i<foundActors.Num(); i++)
	{
		patrolPoint = Cast<APatrolPoint>(foundActors[i]);
		if (patrolPoint->zone == pawn->patrolZone)
		{
			patrolPoints.Add(patrolPoint);
		}
	}
	takeAction();
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

void ACatAIController::setSpookMode(int mode)
/* Whether spooked or not. */
{
	spookMode = mode;
	if (spookMode == spooked)
	{
		GetWorldTimerManager().ClearTimer(moveTimerHandle);
	}
	takeAction();
}

void ACatAIController::setSpookMode(int mode, FVector location)
/* Whether spooked or not and the location of the source of the spook.
 * location not used. Always simply flee from location of player character.
 */
{
	spookMode = mode;
	if (spookMode == spooked)
	{
		GetWorldTimerManager().ClearTimer(moveTimerHandle);
	}
	takeAction();
}

void ACatAIController::wait()
/* Stand still for a while before taking a new action. */
{
	StopMovement();
	GetWorldTimerManager().SetTimer(moveTimerHandle, this, &ACatAIController::takeAction, 2.0, false);
}

void ACatAIController::paceToRandomPoint()
/* Move to a random nearby position. */
{
choosePatrolPoint:
	if (patrolPoints.Num() < 2)
	{
		// Would be preferable to throw error here, but Unreal does not like regular C++ try-throw-catch.
		UE_LOG(LogTemp, Error, TEXT("Place at least two PatrolPoint_BP in the level for the EnemyCat AI to work properly. Using random movement instead."))
		float direction = FMath::RandRange(0.f, 2*PI);
		float distance = FMath::RandRange(100.f, 300.f);
		target.X = cos(direction) * distance;
		target.Y = sin(direction) * distance;
		target += pawn->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("CAT: Move started to (%f, %f)"), target.X, target.Y);
		MoveToLocation(target, 5);
	}
	else
	{
		APatrolPoint* patrolPointNew = patrolPoints[FMath::RandRange(0, patrolPoints.Num() - 1)];  // pick random patrol point
		if (patrolPointNew == patrolpointCurrent)  // pick new one if this was the same as last time
			goto choosePatrolPoint;
		patrolpointCurrent = patrolPointNew;
		UE_LOG(LogTemp, Warning, TEXT("CAT: Move started to (%f, %f)"), patrolpointCurrent->GetActorLocation().X, patrolpointCurrent->GetActorLocation().Y);
		MoveToActor(patrolpointCurrent, 15);
	}
}

void ACatAIController::paceToPlayer()
/* Move towards player character, but only for a few seconds. */
{
	float direction = FMath::RandRange(0.f, 2*PI);
	float distance = FMath::RandRange(30.f, 60.f);
	target.X = cos(direction) * distance;
	target.Y = sin(direction) * distance;
	target += playerPawn->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move started towards player: (%f, %f)"), target.X, target.Y);
	MoveToLocation(target, 5);
	GetWorldTimerManager().SetTimer(moveTimerHandle, this, &ACatAIController::wait, 2.0, false);
}

void ACatAIController::chasePlayer()
/* Start following the player character. */
{
	MoveToActor(playerPawn, 0);
}

void ACatAIController::flee()
/* Move away from the source of the spook, and with a small randomness to the direction.
 * This randomness prevents the cat from getting stuck in an infinite OnMoveCompleted loop when hitting a wall.
 */
{
	spookLocation = playerPawn->GetActorLocation();  // Simplify and always run away from player.
	FVector directionVector = (pawn->GetActorLocation() - spookLocation) / (pawn->GetActorLocation() - spookLocation).Size();
	float distance = FMath::RandRange(100.f, 250.f);
	target = pawn->GetActorLocation() + directionVector * distance;
	
	float direction = FMath::RandRange(0.f, 2 * PI);
	distance = FMath::RandRange(1.f, 40.f);
	target.X += cos(direction) * distance;
	target.Y += sin(direction) * distance;

	UE_LOG(LogTemp, Warning, TEXT("CAT: Move fleeing from (%f, %f)"), target.X, target.Y);
	MoveToLocation(target);
}

void ACatAIController::takeAction()
{
	StopMovement();
	GetWorldTimerManager().ClearTimer(moveTimerHandle);

	if (spookMode == spooked)
	{
		flee();
	}
	else if (alertMode == chasing && pawn->isInAttackRange())
	{
		pawn->attackBegin();
	}
	else if (alertMode == chasing)
	{
		chasePlayer();
	}
	else if (alertMode == suspicious)
	{
		paceToPlayer();
	}
	else
	{
		paceToRandomPoint();
	}
}

void ACatAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move completed"));
	if (spookMode == spooked)
	{
		takeAction();
	}
	else if (alertMode == chasing)
	{
		/* Default to attacking when done chasing.
		 * This happens both if chase stops because player was reached or if chase 
		 * stopped because no further path to the player.
		 * If the cat knows where the bunny is but can't reach it
		 * he might as well stand and attack in the air and try to be intimidating.
		 */
		pawn->attackBegin();
	}
	else if (alertMode == idle)
	{
		GetWorldTimerManager().SetTimer(moveTimerHandle, this, &ACatAIController::takeAction, 2.0, false);
	}
}
