// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "EverWrestleTrap.h"
#include "EverWrestleMovingTrap.generated.h"

UCLASS()
class EVERWRESTLE_API AEverWrestleMovingTrap : public AEverWrestleTrap
{
	GENERATED_BODY()
	
public:
	AEverWrestleMovingTrap();
	
protected:
	// World Interaction
	UFUNCTION(Server, Reliable, Category = "InGame | World Interaction")
	void Server_HandlePlayerCollision(AActor* PlayerToHandle);

	virtual void HandlePlayerCollision(AActor* PlayerToHandle);

	UFUNCTION()
	virtual void OnKnockoutEnd();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	// World Interaction Internal
	UFUNCTION()
	void OnActorCollisionWithTrap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TrapMesh;
	
	// State
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	bool bHasBeenHit = false;
	
	FTimerHandle KnockedOutTimerHandle;
};