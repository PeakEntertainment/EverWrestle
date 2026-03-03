// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SessionGameMode.generated.h"

class UEverWrestleMapData;

UCLASS()
class EVERWRESTLE_API ASessionGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// Round Management
	UFUNCTION(BlueprintCallable, Category="InGame | Round Management")
	void InitiateServerTravel();

private:
	// Configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame | Round Management", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEverWrestleMapData> MapData;
};