// EverWrestle - A Peak Entertainment Production (2025)


#include "EverWrestleGameMode.h"

#include "AdvancedSessionsLibrary.h"
#include "AdvancedSessionSubsystem.h"
#include "EverWrestleCharacter.h"
#include "EverWrestleGameInstanceInterface.h"
#include "EverWrestleGameState.h"
#include "EverWrestleHUD.h"
#include "EverWrestleMapData.h"
#include "EverWrestlePlayerState.h"
#include "EverWrestleScoringData.h"
#include "GameHUD.h"
#include "GameMapsSettings.h"
#include "GamePlayerController.h"

AEverWrestleGameMode::AEverWrestleGameMode() : MAX_ROUND_TIME_IN_SECONDS(MaxRoundTime * 60)
{
	PrimaryActorTick.bCanEverTick = false;
	bUseSeamlessTravel = true;
}

void AEverWrestleGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	MAX_ROUND_TIME_IN_SECONDS = MaxRoundTime * 60;
}

void AEverWrestleGameMode::AddPointsForPlacement(APlayerController* Player, uint8 Placement)
{
	if (IsValid(Player))
	{
		AddPointsForPlayer(Player, ScoringData->GetPointsForPlacement(Placement));
	}
}

void AEverWrestleGameMode::AddPointsForKnockout(APlayerController* Player, uint8 Knockouts)
{
	if (IsValid(Player))
	{
		AddPointsForPlayer(Player, ScoringData->GetPointsForKnockout(Knockouts));
	}
}

void AEverWrestleGameMode::OnPlayerReady(AGamePlayerController* Player)
{
	if (!CachedAdvancedSubsystem)
	{
		if (const UGameInstance* GI = GetGameInstance())
		{
			CachedAdvancedSubsystem = GI->GetSubsystem<UAdvancedSessionSubsystem>();
		}
	}
	
	ReadyPlayers.Add(Player);

	uint8 TargetPlayers = CachedAdvancedSubsystem->GetMaxPlayersFromSession();
	
	if (ReadyPlayers.Num() >= TargetPlayers)
	{
		OnAllPlayersReadyDelegate.Broadcast();
		PrepareCurrentRound();
	}
}

void AEverWrestleGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld())
	{
		if (!CachedGameState)
		{
			CachedGameState = GetGameState<AEverWrestleGameState>();
		}
	
		if (!CachedAdvancedSubsystem)
		{
			if (const UGameInstance* GI = GetGameInstance())
			{
				CachedAdvancedSubsystem = GI->GetSubsystem<UAdvancedSessionSubsystem>();
			}
		}
	}
}

void AEverWrestleGameMode::StartRound()
{
	SetRoundNumber(CurrentRoundNum);
}

void AEverWrestleGameMode::StartRoundTimer()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	
	if (CachedGameState)
	{
		CachedGameState->MultiCast_StartRound(MAX_ROUND_TIME_IN_SECONDS);
	}
	
	// Start Round Timer
	TimerManager.SetTimer(RoundTimerHandle, this, &AEverWrestleGameMode::EndRound, MAX_ROUND_TIME_IN_SECONDS, false);
	// Start the Synchronization Timer to synchronize the Timer for all Players
	TimerManager.SetTimer(SynchronizationTimerHandle, this, &AEverWrestleGameMode::OnTimerSynchronizationRequest, SynchronizationRequestTime, true);
}

void AEverWrestleGameMode::EndRound()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.ClearAllTimersForObject(this);
		
	if (AEverWrestleGameState* GS = GetGameState<AEverWrestleGameState>())
	{
		GS->MultiCast_EndRound();
	}
		
	TArray<FPlayerScoreEntry> PlayerScores;
		
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const AEverWrestlePlayerState* PS = It->Get()->GetPlayerState<AEverWrestlePlayerState>())
		{
			FText PlayerName = CachedAdvancedSubsystem ? 
								   CachedAdvancedSubsystem->GetSteamName(PS) : 
								   FText::FromString(PS->GetPlayerName());
					
			PlayerScores.Emplace(FPlayerScoreEntry(PlayerName, PS->GetCurrentPoints()));
		}
	}
	
	PlayerScores.Sort([](const FPlayerScoreEntry& A, const FPlayerScoreEntry& B)
	{
		return A.PlayerScore > B.PlayerScore;
	});
	
	if (PlayerScores.Num() > 0 && PlayerScores[0].PlayerScore >= MaxPointsForWin || CurrentRoundNum >= MaxRoundNum)
	{
		bHasFinalWinner = true;
		FPlayerScoreEntry Winner = PlayerScores[0];
	}
		
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AGamePlayerController* PC = Cast<AGamePlayerController>(It->Get()))
		{
			PC->Client_InitiateSwitchToScoreboard(PlayerScores, MaxPointsForWin, bHasFinalWinner);
		}
	}
	
	FTimerHandle NextRoundHandle;
	TimerManager.SetTimer(NextRoundHandle, this, &AEverWrestleGameMode::InitiateServerTravel, TimeUntilNextRound, false);
}

void AEverWrestleGameMode::InitiateServerTravel()
{
	if (bHasFinalWinner) { ReturnToMainMenuHost(); return; }
	
	FString LevelName;
	if (MapData)
	{
		TSoftObjectPtr<UWorld> MapPtr = MapData->GetRandomMapFromPool(this);
		LevelName = MapPtr.ToSoftObjectPath().GetLongPackageName();
	}
	
	if (LevelName.IsEmpty()) { ReturnToMainMenuHost(); return; }
	
	PendingLevelName = LevelName;
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AGamePlayerController* PC = Cast<AGamePlayerController>(It->Get()))
		{
			PC->Client_PrepareForTravel();
		}
	}
	
	if (UWorld* World = GetWorld())
	{
		World->bShouldSimulatePhysics = false;
	}
	
	FTimerHandle TravelTimerHandle;
	GetWorldTimerManager().SetTimer(TravelTimerHandle, this, &AEverWrestleGameMode::ExecuteTravel, 2.f, false);
}

void AEverWrestleGameMode::PrepareCurrentRound()
{
	if (UObject* GI = GetGameInstance(); GI && GI->GetClass()->ImplementsInterface(UEverWrestleGameInstanceInterface::StaticClass()))
	{
		const uint8 PersistantRoundNum = IEverWrestleGameInstanceInterface::Execute_GetPersistantRoundNum(GI);
		CurrentRoundNum = FMath::Clamp(PersistantRoundNum + 1, 0, MaxRoundNum);
		IEverWrestleGameInstanceInterface::Execute_SetPersistantRoundNum(GI, CurrentRoundNum);
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const AGamePlayerController* PC = Cast<AGamePlayerController>(It->Get()))
		{
			if (AGameHUD* HUD = Cast<AGameHUD>(PC->GetHUD()))
			{
				HUD->SwitchToGameUI();
			}
		}
	}
	
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.ClearAllTimersForObject(this);
	TimerManager.SetTimer(BeginRoundHandle, this, &AEverWrestleGameMode::StartRound, StartRoundTime, false);
	
	ReadyPlayers.Empty();
}

void AEverWrestleGameMode::AddPointsForPlayer(APlayerController* TargetPlayer, const int32 RewardedPoints)
{
	if (AEverWrestlePlayerState* PS = TargetPlayer->GetPlayerState<AEverWrestlePlayerState>(); PS && TargetPlayer)
	{
		PS->AddScore(RewardedPoints);
	}
}

void AEverWrestleGameMode::AddPointsForEveryone(const int32 RewardedPoints) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PC = It->Get(); PC && PC->GetPlayerState<AEverWrestlePlayerState>())
		{
			PC->GetPlayerState<AEverWrestlePlayerState>()->AddScore(RewardedPoints);
		}
	}
}

void AEverWrestleGameMode::RemovePointsForPlayer(APlayerController* TargetPlayer, const int32 RemovedPoints)
{
	if (AEverWrestlePlayerState* PS = TargetPlayer->GetPlayerState<AEverWrestlePlayerState>(); PS && TargetPlayer)
	{
		PS->SubScore(RemovedPoints);
	}
}

void AEverWrestleGameMode::RemovePointsForEveryone(const int32 RemovedPoints) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PC = It->Get(); PC && PC->GetPlayerState<AEverWrestlePlayerState>())
		{
			PC->GetPlayerState<AEverWrestlePlayerState>()->SubScore(RemovedPoints);
		}
	}
}

void AEverWrestleGameMode::SetPointsForPlayer(APlayerController* TargetPlayer, const int32 NewPoints)
{
	if (AEverWrestlePlayerState* PS = TargetPlayer->GetPlayerState<AEverWrestlePlayerState>(); PS && TargetPlayer)
	{
		PS->SetScore(NewPoints);
	}
}

void AEverWrestleGameMode::SetPointsForEveryone(const int32 NewPoints) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PC = It->Get(); PC && PC->GetPlayerState<AEverWrestlePlayerState>())
		{
			PC->GetPlayerState<AEverWrestlePlayerState>()->SetScore(NewPoints);
		}
	}
}

void AEverWrestleGameMode::OnTimerSynchronizationRequest() const
{
	const FTimerManager& TimerManager = GetWorldTimerManager();
	if (IsValid(CachedGameState))
	{
		CachedGameState->OnTimeSynchronizationReceived(TimerManager.GetTimerRemaining(RoundTimerHandle));
	}
}

void AEverWrestleGameMode::ExecuteTravel()
{
	if (!PendingLevelName.IsEmpty())
	{
		GetWorld()->ServerTravel(PendingLevelName, true);
		PendingLevelName = "";
	}
}

void AEverWrestleGameMode::SetRoundNumber(const uint8 NewRoundNumber)
{
	CurrentRoundNum = NewRoundNumber;
	
	if (!IsValid(CachedGameState))
	{
		CachedGameState = GetGameState<AEverWrestleGameState>();
	}
		
	if (IsValid(CachedGameState))
	{
		CachedGameState->SetCurrentRoundNumber(NewRoundNumber);
	}
}
