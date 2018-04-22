// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCat.h"
#include "CatAIController.h"
#include "TimerManager.h"


AEnemyCat::AEnemyCat()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCat::BeginPlay()
{
	Super::BeginPlay();
	playerPawn = Cast<ABunnyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	GetWorldTimerManager().SetTimer(visionTimerHandle, this, &AEnemyCat::tickVision, deltaTimeVision, true);
}

void AEnemyCat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AEnemyCat::getAttackReach()
{
	return attackReach;
}
float AEnemyCat::getAttackTime()
{
	return attackTime;
}
bool AEnemyCat::isInAttackRange()
{
	return (GetActorLocation() - playerPawn->GetActorLocation()).Size() <= attackReach;
}

void AEnemyCat::attackBegin()
{
	GetWorldTimerManager().SetTimer(attackTimerHandle, this, &AEnemyCat::attackEnd, attackTime, false);

}void AEnemyCat::attackEnd()
{
	if (isInAttackRange())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTACK: KILLED player"));
		controller->takeAction();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTACK: missed player"));
		controller->takeAction();
	}
}

void AEnemyCat::addAlert(float amount)
{
	alert += amount;
	if (alert > alertMax)
		alert = alertMax;
	else if (alert < alertMin)
		alert = alertMin;
	//FMath::Clamp(alert, alertMin, alertMax);
}

void AEnemyCat::tickVision()
	/* Make linetrace to check if player in sight */
{
	visionTraceStart = GetActorLocation();
	visionTraceEnd = playerPawn->GetActorLocation() + FVector(0, 0, -5);
	visionTraceNormal = (visionTraceEnd - visionTraceStart);
	visionTraceNormal /= (visionTraceEnd - visionTraceStart).Size();
	visionTraceStart += visionTraceNormal * 30;  // buffer to net hit self with trace
	float angle = acos( visionTraceNormal.DotProduct(visionTraceNormal, GetActorForwardVector()) / (visionTraceNormal.Size() * GetActorForwardVector().Size()) ) / PI * 180;
	
	DrawDebugLine(
		GetWorld(),
		visionTraceStart,  // from
		visionTraceEnd,    // to
		FColor(255, 0, 0),
		false,
		0, 0, 2
	);
	GetWorld()->LineTraceSingleByChannel(
		visionTraceHit,  // output
		visionTraceStart,
		visionTraceEnd,
		ECC_WorldDynamic
	);
	
	if (visionTraceHit.GetActor())
	{
		if (visionTraceHit.GetActor() == playerPawn && angle <= 75)
		{
			addAlert(alertIncrease * deltaTimeVision);
			//UE_LOG(LogTemp, Warning, TEXT("I SEE YOU, %s!, angle=%f"), *visionTraceHit.GetActor()->GetName(), angle);
		}
		else
		{
			addAlert(-alertDecay * deltaTimeVision);
			//UE_LOG(LogTemp, Warning, TEXT("I don't see you, %s, angle=%f"), *visionTraceHit.GetActor()->GetName(), angle);
		}
	}
	else {
		// visionTrace should always hit the player pawn if nothing else is blocking
		UE_LOG(LogTemp, Error, TEXT("Not hitting anything, angle=%f"), angle);
	}
	UE_LOG(LogTemp, Warning, TEXT("Alert level: %f"), alert);
	if (!bChaseMode && alert >= alertChasing)
	{
		bChaseMode = true;
		controller->setChaseMode(true);
	}
	else if (bChaseMode && alert < alertChasing)
	{
		bChaseMode = false;
		controller->setChaseMode(false);
	}
}

// Called to bind functionality to input
void AEnemyCat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
