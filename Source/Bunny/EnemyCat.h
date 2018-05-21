// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ACatAIController;  // Forward declaration to avoid infinite recursion
class ABunnyCharacter;
#include "DrawDebugHelpers.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCat.generated.h"


UCLASS()
class BUNNY_API AEnemyCat : public ACharacter
{
	GENERATED_BODY()

protected:
	enum { idle = 0, suspicious = 1, chasing = 2 };
	enum { calm = 0, spooked = 1 };

	float maxWalkSpeed = 220.;
	TArray<float> walkSpeedRatios = { 0.5, 0.75, 1.00 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
	float alert = 0.;
	float alertMin = 0.;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
	float alertSuspicious = 1.;  // threshold for becoming suspicious
	float alertChasing = 2.;  // threshold for starting to chase
	float alertMax= 3.;
	float alertIncrease = 1.60;  // How much alertLevel increases per second when seen
	float alertDecay = 0.25;  // How much alertLevel decreses per second when not seen
	void addAlert(float amount);
	int alertMode = idle;
	int spookMode = calm;

	float spook = 0.;
	float spookMin = 0.;
	float spookSpooked = 1.;
	float spookMax = 3.;
	float spookDecay = 0.40;
	void addSpook(float amount);  // only for decay

	ABunnyCharacter* playerPawn;
	FVector visionTraceStart;
	FVector visionTraceEnd;
	FVector visionTraceNormal;
	FHitResult visionTraceHit;

	virtual void BeginPlay() override;
	
	void tickVision();  // enemy look for player
	float deltaTimeVision = 0.20;
	FTimerHandle visionTimerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
	bool bIsAttacking = false;
	float attackReach = 85.;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
	float attackPerformTime = 0.37 * 0.75;  // time it takes to perform attack from attack initiation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
	float attackRecoveryTime = 0.23 * 0.75;  // time it takes to recover from attack from attack completed
	FTimerHandle attackTimerHandle;
	
public:	
	ACatAIController* controller;  // is set by the controller itself
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		int patrolZone = 0;  // Which patrolPoints to go to. Specify in instance.

	AEnemyCat();
	virtual void Tick(float DeltaTime) override;
	float getAttackReach();
	bool isInAttackRange();
	void attackBegin();
	void attackPerform();
	void attackEnd();
	void addSpook(float amount, FVector location);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
