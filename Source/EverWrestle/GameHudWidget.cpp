// EverWrestle - A Peak Entertainment Production (2025)


#include "GameHudWidget.h"

#include "EverWrestleGameState.h"

void UGameHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	if (!CachedGameState)
	{
		CachedGameState = GetWorld()->GetGameState<AEverWrestleGameState>();
	}
	if (CachedGameState)
	{
		CachedGameState->OnRoundStarted.AddUniqueDynamic(this, &UGameHudWidget::OnRoundStarted);
		CachedGameState->OnRoundTimeSynchronizationRequest.AddUniqueDynamic(this, &UGameHudWidget::UpdateRoundTimerDisplay);
		CachedGameState->OnRoundNumberChanged.AddUniqueDynamic(this, &UGameHudWidget::UpdateRoundNumberDisplay);
		
		float Remaining = CachedGameState->GetCurrentRoundTime();
		if (Remaining > 0.f)
		{
			OnRoundStarted(Remaining);
		}
	}
}

void UGameHudWidget::NativeDestruct()
{
	if (CachedGameState)
	{
		CachedGameState->OnRoundStarted.RemoveDynamic(this, &UGameHudWidget::OnRoundStarted);
	}
	
	UUserWidget::NativeDestruct();
}

void UGameHudWidget::SynchronizeTimerWithServer(const float ServerTime)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	float LocalRemaining = TimerManager.GetTimerRemaining(RoundTimerHandle);
	
	if (FMath::Abs(LocalRemaining - ServerTime) < MinSynchronizationDiff) return;
	
	if (TimerManager.IsTimerActive(RoundTimerHandle))
	{
		TimerManager.SetTimer(RoundTimerHandle, this, &UGameHudWidget::EndRound, ServerTime, false);
		
		UE_LOG(LogTemp, Log, TEXT("The timer was synchronized with the Server Timer."));
	}
}

void UGameHudWidget::OnRoundStarted(const float MaxRoundTime)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	TimerManager.ClearAllTimersForObject(this);
	
	TimerManager.SetTimer(RoundTimerHandle, this, &UGameHudWidget::EndRound, MaxRoundTime, false);
	TimerManager.SetTimer(RoundSecondTimerHandle, this, &UGameHudWidget::OnRoundSecondOver, 1.f, true);
}

void UGameHudWidget::OnRoundSecondOver()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	UpdateRoundTimerDisplay(TimerManager.GetTimerRemaining(RoundTimerHandle));
}

void UGameHudWidget::EndRound() const
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearAllTimersForObject(this);
}
