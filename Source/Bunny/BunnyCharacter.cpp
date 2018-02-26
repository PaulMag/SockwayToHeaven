// Fill out your copyright notice in the Description page of Project Settings.

#include "BunnyCharacter.h"
#include "Bunny.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Components/InputComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ABunnyCharacter::ABunnyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(24.f, 24.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bCanGlide = false;		//Set this to true to test/debug gliding, set to false for actual game
	bIsGliding = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	OurVisibleComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("OurVisibleComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->AirControl = 0.25f;
	GetCharacterMovement()->MaxWalkSpeed = 140.0f;
}

// Called when the game starts or when spawned
void ABunnyCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
}

// Called every frame
void ABunnyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Make linetrace to see if there is a climbable wall
	lineTraceStart = GetActorLocation();
	lineTraceRotation = GetActorRotation();
	lineTraceEnd = lineTraceStart + lineTraceRotation.Vector() * climbReach;
	UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), lineTraceEnd.X, lineTraceEnd.Y, lineTraceEnd.Z);
	DrawDebugLine(  // Show facing direction of character
		GetWorld(),
		lineTraceStart,  // from
		lineTraceEnd,    // to
		FColor(255, 0, 128),
		false,
		0, 0, 10
	);
	GetWorld()->LineTraceSingleByObjectType(
		lineTraceHit,  // output
		lineTraceStart,
		lineTraceEnd,
		ECC_WorldStatic  // TODO: WorldStatic should be replaced with a specifc climbable type.
	);

	// Stop climbing if there is no longer a wall
	if (bIsClimbing)
	{
		if (lineTraceHit.GetActor())  // Still on the wall.
		{
			// Move and rotate the character in case the wall curved
			FVector impactNormalXY = lineTraceHit.ImpactNormal;
			impactNormalXY.Z = 0;  // ignore Z-component of impact normal
			SetActorLocation(lineTraceHit.ImpactPoint + impactNormalXY * climbDistance, true);
			FRotator newDirection = (-lineTraceHit.ImpactNormal).ToOrientationRotator();
			GetController()->SetControlRotation(newDirection);
		}
		else  // No longer on the wall.
		{
			lineTraceStart.Z -= vaultHeight;
			lineTraceEnd.Z -= vaultHeight;
			GetWorld()->LineTraceSingleByObjectType(
				lineTraceHit,
				lineTraceStart,
				lineTraceEnd,
				ECC_WorldStatic  // WorldStatic should be replaced with a specifc climbable type.
			);
			if (lineTraceHit.GetActor())  // Lower part of character still on wall => vault
			{
				vault();
			}
			else
			{
				stopClimbing();
			}
		}
	}
}

// Called to bind functionality to input
void ABunnyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &ABunnyCharacter::toggleClimb);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABunnyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABunnyCharacter::MoveRight);

	//Mouse+Keyboard
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//Joystick
	PlayerInputComponent->BindAxis("TurnRate", this, &ABunnyCharacter::TurnAtRate);
	//Mouse+Keyboard
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//Joystick
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABunnyCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Glide", IE_Pressed, this, &ABunnyCharacter::Glide);
	PlayerInputComponent->BindAction("Glide", IE_Released, this, &ABunnyCharacter::StopGliding);
}

void ABunnyCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABunnyCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABunnyCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (bIsClimbing)
		{
			float deltaZ = GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			AddActorWorldOffset(FVector(0, 0, deltaZ), true);
		}
		else  // is walking
		{
			float deltaX = GetCharacterMovement()->MaxWalkSpeed * Value * GetWorld()->DeltaTimeSeconds;
			AddActorLocalOffset(FVector(deltaX, 0, 0), true);  // TODO: change to WorldOffset when static camera
		}
	}

}

void ABunnyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		if (bIsClimbing)
		{
			float deltaY = GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			AddActorLocalOffset(FVector(0, deltaY, 0), true);
		}
		else  // is walking
		{
			float deltaY = GetCharacterMovement()->MaxWalkSpeed * Value * GetWorld()->DeltaTimeSeconds;
			AddActorLocalOffset(FVector(0, deltaY, 0), true);  // TODO: change to WorldOffset when static camera
		}
	}
}


void ABunnyCharacter::toggleClimb()
{
	if (!bCanClimb)
	{
		return;  // If can't climb yet return without doing anything.
	}
	if (bIsClimbing)
	{
		stopClimbing();
		return;
	}
	
	// See what we hit
	AActor* hitActor = lineTraceHit.GetActor();
	if (hitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit actor: %s\n"), *(hitActor->GetName()));
		startClimbing();
	}
}

void ABunnyCharacter::startClimbing()
{
	bIsClimbing = true;
	GetCharacterMovement()->GravityScale = 0.;
	//AddActorLocalOffset(FVector(0, 0, 65.0), true); 
	/* TODO: This shouldn't be necessary, but elsewise character is not able to start climbing.
	*      It is only necessary when the CharacterMovement component is used for movement.
	*/
	GetCharacterMovement()->Velocity = FVector(0, 0, 0);  // Stop any falling movement when grabbing.
	UE_LOG(LogTemp, Warning, TEXT("Climbing ON"));
}

void ABunnyCharacter::stopClimbing()
{
	bIsClimbing = false;  // stop climbing
	GetCharacterMovement()->GravityScale = 1.;
	UE_LOG(LogTemp, Warning, TEXT("Climbing OFF"));
}

void ABunnyCharacter::vault()
/* Jump up and forwards to get on top of the wall. */
{
	AddActorLocalOffset(FVector(2*climbDistance, 0, vaultHeight), false);
	stopClimbing();
}

void ABunnyCharacter::Glide()
{
	if (bCanGlide == true)
	{
		bIsGliding = true;
		GetCharacterMovement()->GravityScale = 0.5f;
		GetCharacterMovement()->AirControl = 1.0f;
	}
}

void ABunnyCharacter::StopGliding()
{
	if (bCanGlide == true)
	{
		GetCharacterMovement()->GravityScale = 1.0f;
		GetCharacterMovement()->AirControl = 0.25f;
		bIsGliding = false;
	}
}
