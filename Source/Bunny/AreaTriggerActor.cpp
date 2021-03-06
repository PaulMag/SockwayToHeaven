// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaTriggerActor.h"


// Sets default values
AAreaTriggerActor::AAreaTriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AAreaTriggerActor::GetbIsOverlappable()
{
	return bIsOverlappable;
}

bool AAreaTriggerActor::GetbIsDestructible()
{
	return bIsDestructible;
}

FString AAreaTriggerActor::RetrieveName()
{
	return name;
}


// Called when the game starts or when spawned
void AAreaTriggerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAreaTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

