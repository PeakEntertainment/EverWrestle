// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "EverWrestleTrap.h"
#include "EverWrestleBearTrap.generated.h"

class UCapsuleComponent;

UCLASS()
class EVERWRESTLE_API AEverWrestleBearTrap : public AEverWrestleTrap
{
	GENERATED_BODY()
	
public:	
	AEverWrestleBearTrap();

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	// World Interaction
	UFUNCTION(Server, Reliable, Category = "InGame | World Interaction")
	void Server_SnapBearTrap(AActor* ActorToKnockout);
	
	UFUNCTION(NetMulticast, Reliable, Category = "InGame | World Interaction")
	void Multicast_SnapBearTrap();
	
	UFUNCTION(NetMulticast, Reliable, Category = "InGame | World Interaction")
	void Multicast_ResetTrap();

	UFUNCTION()
	void ResetTrap();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TrapMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> TrapCollider;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	bool bHasTrapSnapped = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	float SnapDuration = 4.f;
	
	FTimerHandle BearSnapTimerHandle;
};