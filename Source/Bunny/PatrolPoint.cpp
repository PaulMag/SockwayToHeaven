// Fill out your copyright notice in the Description page of Project Settings.

#include "PatrolPoint.h"


APatrolPoint::APatrolPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APatrolPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void APatrolPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

