// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCat.h"
#include "CatAIController.h"
#include "BunnyCharacter.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"


AEnemyCat::AEnemyCat()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCat::BeginPlay()
{
	Super::BeginPlay();
	playerPawn = Cast<ABunnyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	GetWorldTimerManager().SetTimer(visionTimerHandle, this, &AEnemyCat::tickVision, deltaTimeVision, true);
	GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed * walkSpeedRatios[idle];
}

void AEnemyCat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	addSpook(-spookDecay * DeltaTime);
}

float AEnemyCat::getAttackReach()
{
	return attackReach;
}
bool AEnemyCat::isInAttackRange()
{
	return (GetActorLocation() - playerPawn->GetActorLocation()).Size() <= attackReach;
}

void AEnemyCat::attackBegin()
{
	bIsAttacking = true;
	GetWorldTimerManager().SetTimer(attackTimerHandle, this, &AEnemyCat::attackPerform, attackPerformTime, false);
}

void AEnemyCat::attackPerform()
{
	if (isInAttackRange())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTACK: KILLED player"));
		playerPawn->Death();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTACK: missed player"));
	}
	GetWorldTimerManager().SetTimer(attackTimerHandle, this, &AEnemyCat::attackEnd, attackRecoveryTime, false);
}

void AEnemyCat::attackEnd()
{
	bIsAttacking = false;
	controller->takeAction();
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

void AEnemyCat::addSpook(float amount)
/* Magnitude of spookiness and the location of the source of the spook. */
{
	spook += amount;
	if (spook > spookMax)
		spook = spookMax;
	else if (spook < spookMin)
		spook = spookMin;

	int newSpookMode;
	if (spook >= spookSpooked)
	{
		newSpookMode = spooked;
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed * walkSpeedRatios[chasing];
			// always max speed when spooked
	}
	else
	{
		newSpookMode = calm;
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed * walkSpeedRatios[alertMode];
	}

	if (newSpookMode != spookMode)  // only update if spookMode has changed
	{
		spookMode = newSpookMode;
		UE_LOG(LogTemp, Warning, TEXT("Cat changed SPOOKMode to %d"), spookMode);
		controller->setSpookMode(newSpookMode);
	}
}

void AEnemyCat::addSpook(float amount, FVector location)
/* Magnitude of spookiness and the location of the source of the spook. */
{
	spook += amount;
	if (spook > spookMax)
		spook = spookMax;
	else if (spook < spookMin)
		spook = spookMin;

	int newSpookMode;
	if (spook >= spookSpooked)
	{
		newSpookMode = spooked;
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed * walkSpeedRatios[chasing];
			// always max speed when spooked
	}
	else
	{
		newSpookMode = calm;
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed * walkSpeedRatios[alertMode];
	}

	if (newSpookMode != spookMode)  // only update if spookMode has changed
	{
		spookMode = newSpookMode;
		UE_LOG(LogTemp, Warning, TEXT("Cat changed SPOOKMode to %d"), spookMode);
	}
	controller->setSpookMode(newSpookMode, location);
}

void AEnemyCat::tickVision()
	/* Make linetrace to check if player in sight */
{
	visionTraceStart = GetActorLocation();
	visionTraceEnd = playerPawn->GetActorLocation() + FVector(0, 0, -5);
	visionTraceNormal = (visionTraceEnd - visionTraceStart);
	visionTraceNormal /= (visionTraceEnd - visionTraceStart).Size();
	visionTraceStart += visionTraceNormal * 60;  // buffer to not hit self with trace
	float angle = acos( visionTraceNormal.DotProduct(visionTraceNormal, GetActorForwardVector()) / (visionTraceNormal.Size() * GetActorForwardVector().Size()) ) / PI * 180;
	
	/*
	DrawDebugLine(
		GetWorld(),
		visionTraceStart,  // from
		visionTraceEnd,    // to
		FColor(255, 0, 0),
		false,
		0, 0, 2
	);
	*/
	GetWorld()->LineTraceSingleByChannel(
		visionTraceHit,  // output
		visionTraceStart,
		visionTraceEnd,
		ECC_WorldDynamic
	);
	
	if (visionTraceHit.GetActor())
	{
		if (visionTraceHit.GetActor() == playerPawn && angle <= 85)
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
	//UE_LOG(LogTemp, Warning, TEXT("Alert level: %f"), alert);

	int newAlertMode;
	if (alert >= alertChasing)
		newAlertMode = chasing;
	else if (alert >= alertSuspicious)
		newAlertMode = suspicious;
	else
		newAlertMode = idle;
	if (newAlertMode != alertMode)  // only update if alertMode has changed
	{
		alertMode = newAlertMode;
		controller->setAlertMode(newAlertMode);
		if (spookMode != spooked)  // WalkSpeed is set elsewhere when spooked
		{
			GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed * walkSpeedRatios[alertMode];
		}
		UE_LOG(LogTemp, Warning, TEXT("Cat changed alertMode to %d"), alertMode);
	}
}

// Called to bind functionality to input
void AEnemyCat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
