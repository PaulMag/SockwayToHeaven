// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCat.h"


AEnemyCat::AEnemyCat()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCat::BeginPlay()
{
	Super::BeginPlay();
	playerPawn = Cast<ABunnyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AEnemyCat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (checkVision())
	{
		alert += 0.5 * DeltaTime;
	}
	else
	{
		alert -= alertDecay * DeltaTime;
	}
	UE_LOG(LogTemp, Warning, TEXT("Alert level: %f"), alert);
}

bool AEnemyCat::checkVision()
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
			UE_LOG(LogTemp, Warning, TEXT("I SEE YOU, %s!, angle=%f"), *visionTraceHit.GetActor()->GetName(), angle);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("I don't see you, %s, angle=%f"), *visionTraceHit.GetActor()->GetName(), angle);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Not hitting anything, angle=%f"), angle);
	}
	return false;
}

// Called to bind functionality to input
void AEnemyCat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
