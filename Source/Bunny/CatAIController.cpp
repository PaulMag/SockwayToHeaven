// Fill out your copyright notice in the Description page of Project Settings.

#include "CatAIController.h"


ACatAIController::ACatAIController()
{
}

void ACatAIController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY"));
	pawn = Cast<AEnemyCat>(GetPawn());
	MoveToLocation(pawn->target, 125.);
	UE_LOG(LogTemp, Warning, TEXT("MOVE STARTED"));
}

void ACatAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACatAIController::OnMoveCompleted(uint32 RequestID, EPathFollowingResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("MOVE COMPLETED"));
}