// EverWrestle - A Peak Entertainment Production (2025)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EverWrestleGameMode.generated.h"

class AGamePlayerController;
class UEverWrestleMapData;
class UEverWrestleScoringData;
class UAdvancedSessionSubsystem;
class AEverWrestleCharacter;
class AEverWrestleGameState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayerReadyDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerSynchronizationRequestDelegate, float, NewTime);

UCLASS()
class EVERWRESTLE_API AEverWrestleGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AEverWrestleGameMode();
	virtual void PostInitializeComponents() override;

	// Round Management
	UFUNCTION(BlueprintCallable, Category = "InGame | Round Management")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "InGame | Round Management")
	void EndRound();

	UFUNCTION(BlueprintCallable, Category = "InGame | Round Management")
	void PrepareCurrentRound();
	
	UFUNCTION(BlueprintCallable, Category = "InGame | Round Management")
	void StartRoundTimer();

	UFUNCTION(BlueprintCallable, Category = "InGame | Round Management")
	FORCEINLINE uint8 GetCurrentRound() const { return CurrentRoundNum; }
	
	// Scoring & Progression
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	void AddPointsForPlacement(APlayerController* Player, uint8 Placement);
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	void AddPointsForKnockout(APlayerController* Player, uint8 Knockouts);
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	void AddPointsForPlayer(APlayerController* TargetPlayer, int32 RewardedPoints);
	UFUNCTION(BlueprintCallable, Category = "InGame | Scoring & Progression")
	void SetPointsForPlayer(APlayerController* TargetPlayer, int32 NewPoints);
	
	// Point Management
	UFUNCTION(BlueprintCallable, Category = "InGame | Point Management")
	void AddPointsForEveryone(int32 RewardedPoints) const;
	UFUNCTION(BlueprintCallable, Category = "InGame | Point Management")
	void RemovePointsForPlayer(APlayerController* TargetPlayer, int32 RemovedPoints);
	UFUNCTION(BlueprintCallable, Category = "InGame | Point Management")
	void RemovePointsForEveryone(int32 RemovedPoints) const;
	UFUNCTION(BlueprintCallable, Category = "InGame | Point Management")
	void SetPointsForEveryone(int32 NewPoints) const;

	// Session Logistics
	UFUNCTION(BlueprintCallable, Category = "Networking | Session Logistics")
	void OnPlayerReady(AGamePlayerController* Player);
	
	UPROPERTY(BlueprintAssignable, Category = "Networking | Session Logistics")
	FOnAllPlayerReadyDelegate OnAllPlayersReadyDelegate;

protected:
	virtual void BeginPlay() override;

	// Round Management Internal
	UFUNCTION(BlueprintCallable, Category = "InGame | Round Management")
	void InitiateServerTravel();

	void SetRoundNumber(uint8 NewRoundNumber);

private:
	// Timer Management Callbacks
	UFUNCTION()
	void OnTimerSynchronizationRequest() const;

	UPROPERTY(BlueprintAssignable, Category = "InGame | Round Management")
	FOnTimerSynchronizationRequestDelegate OnTimerSyncRequestDelegate;
	
	UFUNCTION()
	void ExecuteTravel();
	
	FString PendingLevelName;

	// Configuration Timer
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="InGame | Round Management", meta = (AllowPrivateAccess, Units = "s"))
	float StartRoundTime = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="InGame | Round Management", meta = (AllowPrivateAccess, Units = "min"))
	float MaxRoundTime = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame | Round Management", meta = (AllowPrivateAccess, Units = "s"))
	float SynchronizationRequestTime = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame | Round Management", meta = (AllowPrivateAccess, Units = "s"))
	float TimeUntilNextRound = 20.f;
	
	// Configuration Rules
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="InGame | Round Management", meta = (AllowPrivateAccess))
	uint8 MaxRoundNum = 5;
	UPROPERTY(BlueprintReadOnly, Category ="InGame | Round Management", meta = (AllowPrivateAccess))
	uint8 CurrentRoundNum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="InGame | Scoring & Progression", meta = (AllowPrivateAccess))
	int32 MaxPointsForWin = 1000;

	// Data Assets
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame | Scoring & Progression", meta = (AllowPrivateAccess))
	TObjectPtr<UEverWrestleScoringData> ScoringData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame | Round Management", meta = (AllowPrivateAccess))
	TObjectPtr<UEverWrestleMapData> MapData;
	
	// Internal State
	UPROPERTY()
	TObjectPtr<AEverWrestleGameState> CachedGameState;
	UPROPERTY()
	TObjectPtr<UAdvancedSessionSubsystem> CachedAdvancedSubsystem;
	UPROPERTY()
	TSet<TObjectPtr<AGamePlayerController>> ReadyPlayers;

	FTimerHandle RoundTimerHandle;
	FTimerHandle SynchronizationTimerHandle;
	FTimerHandle BeginRoundHandle;
	float MAX_ROUND_TIME_IN_SECONDS;
	bool bHasFinalWinner = false;
};