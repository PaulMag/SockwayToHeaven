// Fill out your copyright notice in the Description page of Project Settings.

#include "BunnyCharacter.h"
#include "Bunny.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Components/InputComponent.h"


// Sets default values
ABunnyCharacter::ABunnyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));

	OurVisibleComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ABunnyCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	PlayerController->SetViewTarget(IsometricCamera);
	

	/*
	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;
	*/

	InputComponent->BindAction("Jump", IE_Pressed, this, &ABunnyCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ABunnyCharacter::Fall);
	
	InputComponent->BindAxis("MoveX", this, &ABunnyCharacter::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &ABunnyCharacter::Move_YAxis);
	InputComponent->BindAxis("RotateZ", this, &ABunnyCharacter::RotateZ);
	
	
}

// Called every frame
void ABunnyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Movement(DeltaTime);
	
}

// Called to bind functionality to input
void ABunnyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABunnyCharacter::Move_XAxis(float AxisValue)
{
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * MovementSpeed;
}

void ABunnyCharacter::Move_YAxis(float AxisValue)
{
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * MovementSpeed;
}

void ABunnyCharacter::Jump()
{
	if (CurrentVelocity.Z == 0)
	{
		CurrentVelocity.Z = MovementSpeed;
	}
}

void ABunnyCharacter::Fall()
{
	CurrentVelocity.Z = -(MovementSpeed / 9.8);
}

void ABunnyCharacter::RotateZ(float AxisValue)
{
	CurrentRotation = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void ABunnyCharacter::Movement(float DeltaTime)
{
	if (!CurrentVelocity.IsZero())
	{
		TimeAccellerating += DeltaTime;
		if (TimeAccellerating >= TimeBeforeAccelerate)
		{
			SpeedScale += 0.05;
			SpeedScale > 5.0f ? SpeedScale = 4.0f : SpeedScale;
		}
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * SpeedScale * DeltaTime);
		SetActorLocation(NewLocation);
	}
	else
	{
		SpeedScale = 2.5f;
		TimeAccellerating = 0.f;
	}
	FRotator TempRotation = GetActorRotation();
	TempRotation.Yaw += CurrentRotation;
	SetActorRotation(TempRotation);

}

