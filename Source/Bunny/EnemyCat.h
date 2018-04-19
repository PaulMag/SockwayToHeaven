// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	float alertDecay = 0.1;  // How much alertLevel decreses per second when not seen

	ABunnyCharacter* playerPawn;
	FVector visionTraceStart;
	FVector visionTraceEnd;
	FVector visionTraceNormal;
	FHitResult visionTraceHit;

	virtual void BeginPlay() override;
	
	void tickVision();  // enemy look for player
	float deltaTimeVision = 0.5;
	FTimerHandle visionTimerHandle;
	
public:	
	AEnemyCat();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
	FVector target;
};