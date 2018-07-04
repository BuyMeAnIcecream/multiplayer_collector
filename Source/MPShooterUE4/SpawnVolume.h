// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Pickup.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"


UCLASS()
class MPSHOOTERUE4_API ASpawnVolume : public AActor
{
	GENERATED_BODY()	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	//toggle spawning of pickups on and off
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetSpawningActive(bool bShouldSpawn);

protected:
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class APickup> WhatToSpawn;

	FTimerHandle SpawnTimer;
	//lower spawn time boundary
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeLow;

	//upper ...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeHigh;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", Meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhereToSpawn;

	void SpawnPickup();

	float SpawnDelay;

};