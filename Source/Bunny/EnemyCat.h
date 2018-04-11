// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCat.generated.h"

class UPawnSensingComponent;


UCLASS()
class BUNNY_API AEnemyCat : public ACharacter
{
	GENERATED_BODY()

protected:
	float alert = 0.;
	float alertMin = 0.;
	float alertMax= 3.;
	float alertDecay = 0.1;  // How much alertLevel decreses per second

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* pawnSensingComp;

	void OnPawnSeen(APawn* SeenPawn);
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

public:	
	AEnemyCat();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
