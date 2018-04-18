// Fill out your copyright notice in the Description page of Project Settings.

#include "CatAIController.h"


ACatAIController::ACatAIController()
{
	// Setup the perception component
	perceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	perceptionComponent->ConfigureSense(*sightConfig);
	perceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
	//perceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIControllerUnit::SenseStuff);
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