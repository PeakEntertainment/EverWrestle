// EverWrestle - A Peak Entertainment Production (2025)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHudWidget.generated.h"

class AEverWrestleGameState;

UCLASS()
class EVERWRESTLE_API UGameHudWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void NativeDestruct() override;
	
protected:
	// UI Management
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="InGame | UI Management")
	void UpdateRoundNumberDisplay(uint8 NewRound);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="InGame | UI Management")
	void SynchronizeRoundTimer(float NewTime);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="InGame | UI Management")
	void UpdateRoundTimerDisplay(float NewTime);

private:
	// Internal Timer Logistics
	UFUNCTION(BlueprintCallable, Category="InGame | UI Management", meta = (AllowPrivateAccess = "true"))
	void SynchronizeTimerWithServer(const float ServerTime);
	
	UFUNCTION()
	void OnRoundStarted(const float MaxRoundTime);
	
	UFUNCTION()
	void OnRoundSecondOver();
	
	UFUNCTION()
	void EndRound() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="InGame | UI Management", meta = (AllowPrivateAccess = "true", Units = "s"))
	float MinSynchronizationDiff = 0.5f;
	
	UPROPERTY()
	AEverWrestleGameState* CachedGameState;
	
	FTimerHandle RoundTimerHandle;
	FTimerHandle RoundSecondTimerHandle;
};