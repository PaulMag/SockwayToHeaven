// Fill out your copyright notice in the Description page of Project Settings.

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

void ACatAIController::paceToRandomPoint()
{
	bChaseMode = false;
	float direction = FMath::RandRange(0.f, 2*PI);
	float distance = FMath::RandRange(100.f, 300.f);
	target.X = cos(direction) * distance;
	target.Y = sin(direction) * distance;
	target += pawn->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move started to (%f, %f)"), target.X, target.Y);
	MoveToLocation(target);
}

void ACatAIController::chasePlayer()
{
	bChaseMode = true;
	GetWorldTimerManager().ClearTimer(moveTimerHandle);
	MoveToActor(playerPawn, 0);
}

void ACatAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move completed"));
	if (bChaseMode)
	{
		pawn->attackBegin();
	}
	else
	{
		GetWorldTimerManager().SetTimer(moveTimerHandle, this, &ACatAIController::paceToRandomPoint, 2.0, false);
	}
}
