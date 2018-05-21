// Fill out your copyright notice in the Description page of Project Settings.

#include "BunnyCharacter.h"
#include "Bunny.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "AreaTrigger.h"
#include "CheckpointBase.h"
#include "BunnyGameInstance.h"


// Sets default values
ABunnyCharacter::ABunnyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(24.f, 24.0f);
	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABunnyCharacter::BeginOverlap);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

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
	UWorld* TheWorld = GetWorld();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	FString CurrentLevel = TheWorld->GetMapName();		//Get the name of the level the player is currently playing

	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCat::StaticClass(), foundActors);
	for (int i=0; i<foundActors.Num(); i++)
	{
		enemyPawns.Add(Cast<AEnemyCat>(foundActors[i]));
	}

		if (CurrentLevel == "BunnyTutorialMap")			//Set up to handle things regardless of how player found their way to the level
		{
			bool bCanClimb = false;
			bool bCanGlide = false;
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
			bool bCanClimb = false;
			bool bCanGlide = false;
			bool bCanScare = false;
			UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
			if (BGI)
			{
				BGI->bCanClimb = false;
				BGI->bCanGlide = false;
				BGI->bCanScare = false;
			}
		}
		else if (CurrentLevel == "Level2")
		{
			bool bCanClimb = true;
			bool bCanGlide = false;
			bool bCanScare = false;
			UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
			if (BGI)
			{
				BGI->bCanClimb = true;
				BGI->bCanGlide = false;
				BGI->bCanScare = false;
			}
		}
	UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
	if (BGI)					
	{
		if (BGI->bCanClimb)			//If the player has gained an ability, the game instance is also made aware and stores the progress
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

	// Count scare cooldown
	scareCooldown -= DeltaTime;
	if (scareCooldown < 0)
		scareCooldown = 0;

	// Find movement speed in z-direction for animation purposes
	zPrevious = zCurrent;
	zCurrent = GetActorLocation().Z;
	zDelta = zCurrent - zPrevious;
	zSpeed =  zDelta / DeltaTime;
	if (zSpeed < 0)
	{
		bIsJumping = false;
	}

	// Make linetrace to see if there is a climbable wall
	lineTraceStart = GetActorLocation();
	lineTraceRotation = GetActorRotation();
	lineTraceEnd = lineTraceStart + lineTraceRotation.Vector() * climbReach;
	/*
	DrawDebugLine(  // Show facing direction of character
		GetWorld(),
		lineTraceStart,  // from
		lineTraceEnd,    // to
		FColor(255, 0, 128),
		false,
		0, 0, 2
	);
	*/
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
			FRotator newDirection = (-impactNormalXY).ToOrientationRotator();
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
		else if (GetCharacterMovement()->Velocity.Z > 0) //If moving up (from jump, for instance, stop movement and start gliding)
		{
			GetCharacterMovement()->Velocity.Z = 0;
			Glide();
		}
		else if (GetCharacterMovement()->Velocity.Z == 0)	//Do not glide if player is not falling / in the air
		{
			StopGliding();
		}
	}
}

void ABunnyCharacter::takeDamage(float damage)
{
	health -= damage;
	if (health <= 0)
	{
		health = 0;
		death();
	}
}

void ABunnyCharacter::death()
/* Run this function when BunnyCharacter dies.
 * Deativates controls, plays death animation, and go to deathMenu after a few seconds
 */
{
	if (bIsDead)  // Can't die again.
		return;
	bIsDead = true;
	GetWorldTimerManager().SetTimer(timerHandle, this, &ABunnyCharacter::deathMenu, deathDuration, false);
	
	GetCharacterMovement()->MaxWalkSpeed = 0;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bCanJump = false;
	bIsClimbing = false;
	bCanClimb = false;
	if (bIsVaulting)
		stopVaulting();
	if (bIsGliding)
		StopGliding();
	bCanGlide = false;
	bCanScare = false;
}

// Called to bind functionality to input
void ABunnyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABunnyCharacter::Jump);					//Space
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABunnyCharacter::StopJumping);
	PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &ABunnyCharacter::toggleClimb);		//C
	PlayerInputComponent->BindAction("Glide", IE_Pressed, this, &ABunnyCharacter::Glide);			//Shift
	PlayerInputComponent->BindAction("Glide", IE_Released, this, &ABunnyCharacter::StopGliding);
	PlayerInputComponent->BindAction("Scare", IE_Pressed, this, &ABunnyCharacter::scare);			//Ctrl
	PlayerInputComponent->BindAction("Menu", IE_Pressed, this, &ABunnyCharacter::menu);				//Esc

	PlayerInputComponent->BindAxis("MoveForward", this, &ABunnyCharacter::MoveForward);				//WS | Up,Down
	PlayerInputComponent->BindAxis("MoveRight", this, &ABunnyCharacter::MoveRight);					//AD | Left,Right

	//Mouse+Keyboard
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);					//For future implementations
	//Joystick
	PlayerInputComponent->BindAxis("TurnRate", this, &ABunnyCharacter::TurnAtRate);					//For future implementations
	//Mouse+Keyboard
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);				//For future implementations
	//Joystick
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABunnyCharacter::LookUpAtRate);				//For future implementations

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
			// When climbing, orient control according to orientation of the wall
			float yaw = GetActorRotation().Yaw;
			float deltaY = 0;
			float deltaZ = 0;
			if (yaw >= -45 && yaw < 45)
				deltaZ = GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			else if (yaw >= 45 && yaw < 135)
				deltaY = - GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			else if (yaw >= -135 && yaw < 45)
				deltaY = GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			else
				deltaZ = - GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			AddActorLocalOffset(FVector(0, deltaY, deltaZ), true);
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
		if (bIsClimbing)
		{
			// When climbing, orient control according to orientation of the wall
			float yaw = GetActorRotation().Yaw;
			float deltaY = 0;
			float deltaZ = 0;
			if (yaw >= -45 && yaw < 45)
				deltaY = GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			else if (yaw >= 45 && yaw < 135)
				deltaZ = GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			else if (yaw >= -135 && yaw < 45)
				deltaZ = -GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			else
				deltaY = -GetCharacterMovement()->MaxWalkSpeed * climbSpeedRatio * Value * GetWorld()->DeltaTimeSeconds;
			AddActorLocalOffset(FVector(0, deltaY, deltaZ), true);
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
	FString effect = dynamic_cast<ACheckpointBase*>(OtherActor)->effect;  // Retrieve the effect of the object that was overlapped
	AAreaTrigger *OverlappedActor = dynamic_cast<AAreaTrigger*>(OtherActor);

	if (effect == "climb")		//Treat result of overlapping according to what was encounterd
	{
		UE_LOG(LogTemp, Warning, TEXT("Caterpillar Encountered"));	//Overlaps the Caterpillar
		bCanClimb = true;											//Gives player the ability to climb
		UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
		if (BGI)
		{
			BGI->bCanClimb = true;									//Updates the game instance to also know the player can climb
		}
		if (OverlappedActor->GetbIsDestructible())
		{
			OtherActor->Destroy(); // Destroys the Caterpillar object. It is now "absorbed" into the sock.
		}
	}
	else if (effect == "glide")  // As for Caterpillar, but with Glide
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
	else if (effect == "scare")  // As for Caterpillar, but with Scare
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
	else if (effect == "end")  // Levels are designed to have only one "end" point
	{
		UE_LOG(LogTemp, Warning, TEXT("End Encountered"));
		if (OverlappedActor->GetbIsDestructible())
		{
			OtherActor->Destroy();									//Removes the object
		}
		SwapLevel();												//Figures out where to send the player next
	}
	/*else if (effect == "death")										//If overlapping an object effect death, would "kill" player and show death screen
	{
		UE_LOG(LogTemp, Warning, TEXT("Death Encountered"));
		if (OverlappedActor->GetbIsDestructible())
		{
			OtherActor->Destroy();
		}
		Death();
	}
	*/
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Nothing Recognizable Encountered"));		
	}
}

void ABunnyCharacter::SwapLevel()							//Loads player into levels
{
	UWorld* TheWorld = GetWorld();

	FString CurrentLevel = TheWorld->GetMapName();			//Stores which level the player is currently in

	if (CurrentLevel == "BunnyTutorialMap")					//Tests which level the player is in
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Level1");	//And sends the player to the appropriate level

	}
	else if (CurrentLevel == "Level1")						//If already on Level 1
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Level2");	//Level 2 is next
	}
	else if (CurrentLevel == "Level2")						//If already on Level 2
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Main Menu");	//Game is over and palyer returns on Main meny
	}
	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Main Menu");	//If player ended an unknown map, send them to main menu
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
	SetActorEnableCollision(false);  // Allows character to be close to wall while climbing.
	UE_LOG(LogTemp, Warning, TEXT("Climbing ON"));
}

void ABunnyCharacter::stopClimbing()
{
	bIsClimbing = false;  // stop climbing
	GetCharacterMovement()->MovementMode = MOVE_Walking;
	SetActorEnableCollision(true);
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

void ABunnyCharacter::Jump()
/* If you press Jump while in the air you activate Glide. */
{
	if (GetCharacterMovement()->IsFalling())  // Glide
	{
		Glide();
	}
	else if (bCanJump)  // Normal Jump if not falling
	{
		bIsJumping = true;
		ACharacter::Jump();
	}
}

void ABunnyCharacter::StopJumping()
{
	if (GetCharacterMovement()->IsFalling())
	{
		StopGliding();
	}
}

void ABunnyCharacter::Glide()
{
	if (bCanGlide == true)			//If player has the ability to glide
	{
		if (bIsGliding != true)		//If player is not already gliding
		{
			bIsGliding = true;		//Set the state that the player is gliding
		}
		GetCharacterMovement()->GravityScale = GlideGravityScale;	//And assign the glide values to gravity and air control
		GetCharacterMovement()->AirControl = GlideAirControl;
	}
}

void ABunnyCharacter::StopGliding()
{
	if (bCanGlide == true)			//If player has the ability to glide
	{
		if (bIsGliding == true)		//If player is gliding
		{
			bIsGliding = false;		//The player is no longer gliding
		}
		GetCharacterMovement()->GravityScale = DefaultGravityScale;	//And assigns the default alues to gravity and air control
		GetCharacterMovement()->AirControl = DefaultAirControl;
	}
}

void ABunnyCharacter::scare()
/* Scare away every enemy within certain distance.
 * The scare lasts longer (more "spook") the closer the enemy is, maximum of 5 seconds.
 */
{
	if (bCanScare && scareCooldown <= 0)
	{
		for (int i=0; i<enemyPawns.Num(); i++)
		{
			float distance = (GetActorLocation() - enemyPawns[i]->GetActorLocation()).Size();
			if (distance < scareRangeFull)
			{
				enemyPawns[i]->addSpook(spookAmountFull, GetActorLocation());
			}
			else if (distance < scareRangeMax)
			{
				float spookAmount = (scareRangeMax - distance) / (scareRangeMax - scareRangeFull) * spookAmountFull;
				enemyPawns[i]->addSpook(spookAmount, GetActorLocation());
			}
		}
		scareCooldown = scareCooldownMax;
	}
}

void ABunnyCharacter::menu()
/* Handles if the player pressed "escape" to enter the main menu. */
{
	UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
	if (BGI)
	{
		UWorld* TheWorld = GetWorld();
		FString CurrentLevel = TheWorld->GetMapName();	//Stores what map the player was on before they enter the main menu
		BGI->CurrentLevel = CurrentLevel;				//Stores that name in the game instance so it will carry over map loads
	}
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");	//Then loads the main menu
}

void ABunnyCharacter::deathMenu()
/* Goes to the "You died" menu after death animation has been played. */
{
	UBunnyGameInstance* BGI = Cast<UBunnyGameInstance>(GetGameInstance());
	if (BGI)
	{
		UWorld* TheWorld = GetWorld();
		FString CurrentLevel = TheWorld->GetMapName();
		BGI->CurrentLevel = CurrentLevel;
	}
	UGameplayStatics::OpenLevel(GetWorld(), "DeathMenu");
}
