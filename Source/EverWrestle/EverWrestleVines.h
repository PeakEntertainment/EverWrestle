// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EverWrestleVines.generated.h"

class UBoxComponent;

UCLASS()
class EVERWRESTLE_API AEverWrestleVines : public AActor
{
	GENERATED_BODY()
	
public:	
	AEverWrestleVines();

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
private:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxCollision;
	
	// Combat & Health Configuration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | Combat & Health", meta = (AllowPrivateAccess = "true", Units = "s"))
	float KnockoutDuration = 3.f;
};