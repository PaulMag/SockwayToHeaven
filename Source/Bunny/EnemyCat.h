// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ACatAIController;  // Forward declaration to avoid infinite recursion
#include "BunnyCharacter.h"
#include "DrawDebugHelpers.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCat.generated.h"


UCLASS()
class BUNNY_API AEnemyCat : public ACharacter
{
	GENERATED_BODY()

protected:
	float alert = 0.;
	float alertMin = 0.;
	float alertSuspicious = 1.;  // threshold for becoming suspicious
	float alertChasing = 2.;  // threshold for starting to chase
	float alertMax= 3.;
	float alertIncrease = 0.75;  // How much alertLevel increases per second when seen
	float alertDecay = 0.25;  // How much alertLevel decreses per second when not seen
	void addAlert(float amount);
	bool bChaseMode = false;

	ABunnyCharacter* playerPawn;
	FVector visionTraceStart;
	FVector visionTraceEnd;
	FVector visionTraceNormal;
	FHitResult visionTraceHit;

	virtual void BeginPlay() override;
	
	void tickVision();  // enemy look for player
	float deltaTimeVision = 0.5;
	FTimerHandle visionTimerHandle;
	
	float attackReach = 40.;
	float attackTime = 1.5;  // time it takes to complete attack from attack initiation
	FTimerHandle attackTimerHandle;
	
public:	
	ACatAIController* controller;  // is set by the controller itself
	AEnemyCat();
	virtual void Tick(float DeltaTime) override;
	float getAttackReach();
	float getAttackTime();
	bool isInAttackRange();
	void attackBegin();
	void attackEnd();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
