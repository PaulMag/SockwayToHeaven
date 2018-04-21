// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaTrigger.h"


// Sets default values
AAreaTrigger::AAreaTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AAreaTrigger::GetbIsOverlappable()
{
	return bIsOverlappable;
}

bool AAreaTrigger::GetbIsDestructible()
{
	return bIsDestructible;
}

FString AAreaTrigger::RetrieveName()
{
	return name;
}

// Called when the game starts or when spawned
void AAreaTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAreaTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

