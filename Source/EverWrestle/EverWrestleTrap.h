// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EverWrestleTrap.generated.h"

UCLASS(Abstract)
class EVERWRESTLE_API AEverWrestleTrap : public AActor
{
	GENERATED_BODY()

public:
	AEverWrestleTrap();
	
protected:
	virtual void BeginPlay() override;
	
	// World Interaction Configuration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction")
	TObjectPtr<UAnimationAsset> TrapAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | World Interaction")
	float KnockedOutDuration = 3.f;
};