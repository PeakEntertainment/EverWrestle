// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EverWrestleCrushTrap.generated.h"

UCLASS()
class EVERWRESTLE_API AEverWrestleCrushTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	AEverWrestleCrushTrap();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	// World Interaction
	UFUNCTION(Server, Reliable)
	void Server_HandlePlayerCollision(AActor* PlayerToHandle);

	UFUNCTION()
	void OnActorCollisionWithTrap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnKnockoutEnd();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TrapMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimationAsset> TrapAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	float KnockedOutDuration = 3.f;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	bool bHasBeenHit = false;

	FTimerHandle KnockedOutTimerHandle;
};