// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EverWrestleScoringData.generated.h"

UCLASS(BlueprintType)
class EVERWRESTLE_API UEverWrestleScoringData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Scoring & Progression
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	int32 GetPointsForPlacement(uint8 Placement);
	
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	int32 GetPointsForKnockout(uint8 Knockouts);
	
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	FORCEINLINE TMap<uint8, int32> GetPlacementPointsMap() { return PlacementPointsMap; }
	
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	FORCEINLINE TMap<uint8, int32> GetKnockoutPointsMap() { return KnockoutPointsMap; }
	
private:
	// Scoring Configuration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | Scoring & Progression", meta = (AllowPrivateAccess = "true"))
	TMap<uint8, int32> PlacementPointsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGame | Scoring & Progression", meta = (AllowPrivateAccess = "true"))
	TMap<uint8, int32> KnockoutPointsMap;
	
	int32 GetPointsOutOfMap(const TMap<uint8, int32>& MapToCheck, uint8 Num) const;
};