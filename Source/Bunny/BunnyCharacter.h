// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyCat.h"
#include "DrawDebugHelpers.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BunnyCharacter.generated.h"

UCLASS()
class BUNNY_API ABunnyCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:
	TArray<AEnemyCat*> enemyPawns;

	FVector lineTraceStart;
	FRotator lineTraceRotation;
	FVector lineTraceEnd;
	FHitResult lineTraceHit;

	float climbSpeedRatio = 0.5;
	float climbReach = 50.;  // Max distance to wall to start climbing
	float climbDistance = 30.;  // Distance from wall while climbing.
	bool bIsVaulting = false;
	float vaultHeight = 30;
	float vaultDuration = 0.5;  // How many seconds the vault movement takes
	float vaultTimeRemaining;  // Where in the vault movement the character is

	float scareCooldownMax = 30.;  // cooldown between each use of scare
	float scareCooldown = 0.;  // current cooldown time left
	float spookAmountFull = 3.;  // amount of spook within scareRangeFull
	float scareRangeFull = 100.;  // within this range scare has full effect
	float scareRangeMax = 200.;  // outside this range scare has 0 effect

	void toggleClimb();
	void startClimbing();
	void stopClimbing();
	void startVaulting();
	void stopVaulting();

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);

	UFUNCTION()
		void Menu();

	UFUNCTION()
		void SwapLevel();

	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Glide();
	void StopGliding();
	void scare();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float BaseTurnRate = 45.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float BaseLookUpRate = 45.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float DefaultGravityScale = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float GlideGravityScale = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float DefaultAirControl = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float GlideAirControl = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float DefaultWalkSpeed = 140.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float DefaultJumpSpeed = 280.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float DefaultAcceleration = 140.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		float DefaultDecelaration = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		bool bIsGliding = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables)
		bool bIsClimbing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		bool bCanGlide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		bool bCanClimb = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		bool bCanScare = false;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:	
	ABunnyCharacter();

	virtual void Tick(float DeltaTime) override;

	void Death();

	/*
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	*/

	USceneComponent* CollisionCapsule = nullptr;
};
