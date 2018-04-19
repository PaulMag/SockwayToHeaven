// Fill out your copyright notice in the Description page of Project Settings.

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "CatAIController.h"


ACatAIController::ACatAIController()
{
}

void ACatAIController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("CAT: Begin play"));
	pawn = Cast<AEnemyCat>(GetPawn());
	paceToRandomPoint();
}

void ACatAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
*/
void ACatAIController::paceToRandomPoint()
{
	float direction = FMath::RandRange(0.f, 2*PI);
	float distance = FMath::RandRange(100.f, 300.f);
	target.X = cos(direction) * distance;
	target.Y = sin(direction) * distance;
	target += pawn->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move started to (%f, %f)"), target.X, target.Y);
	MoveToLocation(target);
}

void ACatAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	UE_LOG(LogTemp, Warning, TEXT("CAT: Move completed"));
	GetWorldTimerManager().SetTimer(moveTimerHandle, this, &ACatAIController::paceToRandomPoint, 2.0, false);
}