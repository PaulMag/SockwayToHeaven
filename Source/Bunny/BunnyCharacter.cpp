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
#include "AreaTrigger.h"
#include "BunnyGameInstance.h"


// Sets default values
ABunnyCharacter::ABunnyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(24.f, 24.0f);
	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABunnyCharacter::BeginOverlap);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	OurVisibleComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("OurVisibleComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	GetCharacterMovement()->AirControl = DefaultAirControl;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->JumpZVelocity = DefaultJumpSpeed;
	GetCharacterMovement()->MaxAcceleration = DefaultAcceleration;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultDecelaration;
}

// Called when the game starts or when spawned
void ABunnyCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	UWorld* TheWorld = GetWorld();
	FString CurrentLevel = TheWorld->GetMapName();

		if (CurrentLevel == "BunnyTutorialMap")
		{
			bool bCanGlide = false;
			bool bCanClimb = false;
			bool bCanScare = false;
			UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
			if (BGI)
			{
				BGI->bCanClimb = false;
				BGI->bCanGlide = false;
				BGI->bCanScare = false;
			}
		}
		else if (CurrentLevel == "Level1")
		{
			bool bCanGlide = true;
			bool bCanClimb = false;
			bool bCanScare = false;
			UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
			if (BGI)
			{
				BGI->bCanClimb = true;
				BGI->bCanGlide = false;
				BGI->bCanScare = false;
			}
		}
		else if (CurrentLevel == "Level2")
		{
			bool bCanGlide = true;
			bool bCanClimb = true;
			bool bCanScare = true;
			UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
			if (BGI)
			{
				BGI->bCanClimb = true;
				BGI->bCanGlide = true;
				BGI->bCanScare = true;
			}
		}

	UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
	if (BGI)
	{
		if (BGI->bCanClimb)
			bCanClimb = true;
	}
	if (BGI)
	{
		if (BGI->bCanGlide)
			bCanGlide = true;
	}
	if (BGI)
	{
		if (BGI->bCanScare)
			bCanScare = true;
	}
}

// Called every frame
void ABunnyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Make linetrace to see if there is a climbable wall
	lineTraceStart = GetActorLocation();
	lineTraceRotation = GetActorRotation();
	lineTraceEnd = lineTraceStart + lineTraceRotation.Vector() * climbReach;
	DrawDebugLine(  // Show facing direction of character
		GetWorld(),
		lineTraceStart,  // from
		lineTraceEnd,    // to
		FColor(255, 0, 128),
		false,
		0, 0, 2
	);
	GetWorld()->LineTraceSingleByObjectType(
		lineTraceHit,  // output
		lineTraceStart,
		lineTraceEnd,
		ECC_GameTraceChannel1
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
			SetActorRotation(newDirection);
		}
		else  // No longer on the wall.
		{
			lineTraceStart.Z -= vaultHeight;
			lineTraceEnd.Z -= vaultHeight;
			GetWorld()->LineTraceSingleByObjectType(
				lineTraceHit,
				lineTraceStart,
				lineTraceEnd,
				ECC_GameTraceChannel1
			);
			if (lineTraceHit.GetActor())  // Lower part of character still on wall => vault
			{
				startVaulting();
			}
			else
			{
				stopClimbing();  // else, just fall down
			}
		}
	}
	else if (bIsVaulting)
	{
		AddActorLocalOffset(FVector(1.5*climbDistance, 0, vaultHeight) * DeltaTime / vaultDuration, false);
		vaultTimeRemaining -= DeltaTime;
		if (vaultTimeRemaining <= 0)
		{
			stopVaulting();
		}
	}

	if (bIsGliding == true)
	{
		if (GetCharacterMovement()->Velocity.Z < 0) //Only glide if falling
		{
			Glide();
		}
		else if (GetCharacterMovement()->Velocity.Z >= 0)
		{
			StopGliding();
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
	PlayerInputComponent->BindAction("Glide", IE_Pressed, this, &ABunnyCharacter::Glide);
	PlayerInputComponent->BindAction("Glide", IE_Released, this, &ABunnyCharacter::StopGliding);
	PlayerInputComponent->BindAction("Menu", IE_Pressed, this, &ABunnyCharacter::Menu);

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
		else if (bIsVaulting)
		{
			// do nothing
		}
		else  // is walking
		{
			AddMovementInput(FVector(1, 0, 0), Value);
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
		else if (bIsVaulting)
		{
			// do nothing
		}
		else  // is walking
		{
			AddMovementInput(FVector(0, 1, 0), Value);
		}
	}
}


void ABunnyCharacter::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult &SweepResult)
{
	FString name = OtherActor->GetActorLabel();
	AAreaTrigger *OverlappedActor = dynamic_cast<AAreaTrigger*>(OtherActor);
	//FString name = OverlappedActor->RetrieveName();

	if (name == "Caterpillar")
	{
		UE_LOG(LogTemp, Warning, TEXT("Caterpillar Encountered"));
		bCanClimb = true;	
		UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
		if (BGI)
		{
			BGI->bCanClimb = true;
		}
		if (OverlappedActor->GetbIsDestructible())
		{
			OtherActor->Destroy();
		}
	}
	else if (name == "Squirrel")
	{
		UE_LOG(LogTemp, Warning, TEXT("Squirrel Encountered"));
		bCanGlide = true;
		UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
		if (BGI)
		{
			BGI->bCanGlide = true;
		}
		if (OverlappedActor->GetbIsDestructible())
		{
			OtherActor->Destroy();
		}
	}
	else if (name == "Skunk")
	{
		bCanScare = true;
		UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
		if (BGI)
		{
		BGI->bCanScare = true;
		}
		if (OverlappedActor->GetbIsDestructible())
		{
			OtherActor->Destroy();
		}
	}
	else if (name == "End")
	{
		UE_LOG(LogTemp, Warning, TEXT("End Encountered"));
		if (OverlappedActor->GetbIsDestructible())
		{
			OtherActor->Destroy();
		}
		SwapLevel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Nothing Recognizable Encountered"));
	}
}

void ABunnyCharacter::SwapLevel()
{
	UWorld* TheWorld = GetWorld();

	FString CurrentLevel = TheWorld->GetMapName();

	if (CurrentLevel == "BunnyTutorialMap")
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Level1");

	}
	else if (CurrentLevel == "Level1")
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Level2");
	}
	else if (CurrentLevel == "Level2")
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Main Menu");
	}
	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), "BunnyTutorialMap");
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
	GetCharacterMovement()->MovementMode = MOVE_Flying;
	GetCharacterMovement()->Velocity = FVector(0, 0, 0);  // Stop any falling movement when grabbing.
	UE_LOG(LogTemp, Warning, TEXT("Climbing ON"));
}

void ABunnyCharacter::stopClimbing()
{
	bIsClimbing = false;  // stop climbing
	GetCharacterMovement()->MovementMode = MOVE_Walking;
	UE_LOG(LogTemp, Warning, TEXT("Climbing OFF"));
}

void ABunnyCharacter::startVaulting()
/* Jump up and forwards to get on top of the wall.
 * The actual movement happens in Tick while bIsVaulting is true.
 */
{
	stopClimbing();
	SetActorEnableCollision(false);
	bIsVaulting = true;
	vaultTimeRemaining = vaultDuration;
	GetCharacterMovement()->MovementMode = MOVE_Flying;
	UE_LOG(LogTemp, Warning, TEXT("Vaulting ON"));
}

void ABunnyCharacter::stopVaulting()
{
	bIsVaulting = false;
	SetActorEnableCollision(true);
	GetCharacterMovement()->MovementMode = MOVE_Walking;
	UE_LOG(LogTemp, Warning, TEXT("Vaulting OFF"));
}

void ABunnyCharacter::Glide()
{
	if (bCanGlide == true)
	{
		if (bIsGliding != true)
		{
			bIsGliding = true;
		}
		GetCharacterMovement()->GravityScale = GlideGravityScale;
		GetCharacterMovement()->AirControl = GlideAirControl;
	}
}

void ABunnyCharacter::StopGliding()
{
	if (bCanGlide == true)
	{
		if (bIsGliding == true)
		{
			bIsGliding = false;
		}
		GetCharacterMovement()->GravityScale = DefaultGravityScale;
		GetCharacterMovement()->AirControl = DefaultAirControl;
	}
}

void ABunnyCharacter::Menu()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}
