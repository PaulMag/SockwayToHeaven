// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AreaTrigger.generated.h"

UCLASS()
class BUNNY_API AAreaTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAreaTrigger();

	bool GetbIsOverlappable();
	bool GetbIsDestructible();
	FString RetrieveName();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
		bool bIsOverlappable = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
		bool bIsDestructible = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
		FString name = "AreaTrigger";

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
