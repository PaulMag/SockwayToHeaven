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
}

// Called to bind functionality to input
void ABunnyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

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
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

}

void ABunnyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
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

