// EverWrestle - A Peak Entertainment Production (2025)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Deathzone.generated.h"

class UBoxComponent;

UCLASS()
class EVERWRESTLE_API ADeathzone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeathzone();

protected:
	virtual void BeginPlay() override;

private:
	// World Interaction Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxCollision;
};