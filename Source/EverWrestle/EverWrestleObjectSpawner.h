// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InitiateSelfDestructInterface.h"
#include "EverWrestleObjectSpawner.generated.h"

class UArrowComponent;
class UBoxComponent;

UCLASS()
class EVERWRESTLE_API AEverWrestleObjectSpawner : public AActor
{
	GENERATED_BODY()
	
public:
	AEverWrestleObjectSpawner();

	// World Interaction
	UFUNCTION(Server, Reliable, Category = "InGame | World Interaction")
	void Server_StartSpawnCycle();

protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void SpawnObjects();
	FVector GetRandomSpawnLocationInsideCube() const;
	FRotator GetRandomRotation() const;
	FVector GetRandomScale(float MinObjectScale, float MaxObjectScale) const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	float SpawnInterval = 4.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	uint8 MinObjectSpawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	uint8 MaxObjectSpawn = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	float ObjectDespawnDuration = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	float ImpulseStrength = 1750.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	float MinObjectScale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	float MaxObjectScale = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AActor>> ObjectsToSpawn;

	FTimerHandle SpawnTimerHandle;
};