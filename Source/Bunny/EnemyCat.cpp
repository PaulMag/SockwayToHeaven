// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCat.h"
#include "Perception/PawnSensingComponent.h"


// Sets default values
AEnemyCat::AEnemyCat()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	pawnSensingComp->bEditableWhenInherited = true;
}

// Called when the game starts or when spawned
void AEnemyCat::BeginPlay()
{
	Super::BeginPlay();
	pawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCat::OnPawnSeen);
	pawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyCat::OnNoiseHeard);
}

// Called every frame
void AEnemyCat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	alert -= alertDecay * DeltaTime;
}

// Called to bind functionality to input
void AEnemyCat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCat::OnPawnSeen(APawn * SeenPawn)
{
	alert += 1 * GetWorld()->DeltaTimeSeconds;
	target = SeenPawn->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("PLAYER SPOTTED"));
}

void AEnemyCat::OnNoiseHeard(APawn * NoiseInstigator, const FVector & Location, float Volume)
{
	alert += 1 * GetWorld()->DeltaTimeSeconds;
}
