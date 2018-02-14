// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BunnyCharacter.generated.h"

UCLASS()
class BUNNY_API ABunnyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABunnyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* OurVisibleComponent;

	USceneComponent* CollisionCapsule = nullptr;

	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	
	FVector CurrentVelocity;
	
	UPROPERTY(EditAnywhere)
		float SpeedScale = 1.f;
		float TimeBeforeAccelerate = 1.f;
		float TimeAccellerating{ 0.f };
		float MovementSpeed = 140.0f;

	/*UPROPERTY(EditAnywhere)
		ACameraActor* PlayerCamera = nullptr;
	*/
private:
	void Movement(float DeltaTime);
};
