#include "AdvancedSessionSubsystem.h"

#include "AdvancedSteamFriendsLibrary.h"
#include "BlueprintDataDefinitions.h"
#include "FindSessionsCallbackProxy.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

void UAdvancedSessionSubsystem::CreateGameSession(const int32 MaxPlayers, bool bIsLan, bool bAllowInvites, bool bIsDedicated, bool bUseLobbiesIfAvailable, bool bUsePresence, bool bAllowJoinViaPresence, bool bAllowJoinViaPresenceFriendsOnly, bool bAntiCheatProtected, bool bUsesStats, bool bShouldAdvertise, bool bUseLobbiesVoiceChatIfAvailable, bool bAllowJoinInProgress, const FString ServerName, const FString ServerPassword)
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM);
    if (!Subsystem) return;
    
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        OnSessionCreationComplete.Broadcast(false);
        return;
    }
    
    FOnlineSessionSettings Settings;
    Settings.NumPublicConnections = MaxPlayers;
    Settings.NumPrivateConnections = 0;
    Settings.bIsLANMatch = bIsLan;
    Settings.bAllowInvites = bAllowInvites;
    Settings.bIsDedicated = bIsDedicated;
    Settings.bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
    Settings.bUsesPresence = bUsePresence;
    Settings.bAllowJoinViaPresence = bAllowJoinViaPresence;
    Settings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
    Settings.bAntiCheatProtected = bAntiCheatProtected;
    Settings.bUsesStats = bUsesStats;
    Settings.bShouldAdvertise = bShouldAdvertise;
    Settings.bUseLobbiesVoiceChatIfAvailable = bUseLobbiesVoiceChatIfAvailable;
    Settings.bAllowJoinInProgress = bAllowJoinInProgress;
    
    Settings.Set(FName("GameType"), FString("GameSession"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    Settings.Set(FName("ServerName"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    
    // Only set the Password if it is not empty (Subsystem will not save empty Strings)
    if (!ServerPassword.IsEmpty())
        Settings.Set(FName("ServerPassword"), ServerPassword, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    
    if (CreateSessionsHandle.IsValid())
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionsHandle);
    
    CreateSessionsHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UAdvancedSessionSubsystem::HandleCreateSessionComplete));
    
    if (SessionInterface->GetNamedSession(NAME_GameSession))
    {
        SessionInterface->DestroySession(NAME_GameSession);
    }
    
    SessionInterface->CreateSession(0, NAME_GameSession, Settings);
}

void UAdvancedSessionSubsystem::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    OnSessionCreationComplete.Broadcast(bWasSuccessful);
}

void UAdvancedSessionSubsystem::JoinGameSession(const FBlueprintSessionResult& SessionToJoin)
{
    const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM);
    if (!Subsystem) return;

    const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        OnSessionJoinComplete.Broadcast(false);
        return;
    }
    
    if (JoinSessionsHandle.IsValid())
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionsHandle);
    
    JoinSessionsHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UAdvancedSessionSubsystem::HandleJoinSessionComplete));
    
    SessionInterface->JoinSession(0, NAME_GameSession, SessionToJoin.OnlineResult);
}

void UAdvancedSessionSubsystem::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        if (const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM))
        {
            const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
            if (FString TravelURL; SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
            {
                if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
                {
                    PlayerController->ClientTravel(TravelURL, TRAVEL_Absolute);
                    OnSessionJoinComplete.Broadcast(true);
                    return;
                }
            }
        }
    }
    OnSessionJoinComplete.Broadcast(false);
}

void UAdvancedSessionSubsystem::FindGameSessions(const int32 MaxResults, const bool bIsLan)
{
    const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM);
    if (!Subsystem) return;

    const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        OnSessionFindComplete.Broadcast({}, false);
        return;
    }
    
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->MaxSearchResults = MaxResults;
    SessionSearch->bIsLanQuery = bIsLan;
    SessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals);
    SessionSearch->QuerySettings.Set(FName("GameType"), FString("GameSession"), EOnlineComparisonOp::Equals);
    
    if (FindSessionsHandle.IsValid())
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsHandle);
    
    FindSessionsHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UAdvancedSessionSubsystem::HandleFindSessionComplete));
    
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UAdvancedSessionSubsystem::HandleFindSessionComplete(const bool bWasSuccessful)
{
    TArray<FBlueprintSessionResult> BlueprintResults;
    
    if (bWasSuccessful && SessionSearch.IsValid())
    {
        for (const auto& SearchResult : SessionSearch->SearchResults)
        {
            BlueprintResults.Add(FBlueprintSessionResult(SearchResult));
        }
    }
    
    OnSessionFindComplete.Broadcast(BlueprintResults, bWasSuccessful);
}

void UAdvancedSessionSubsystem::GetSessionProperty(const FBlueprintSessionResult& SessionResult, const FName Key,
    FString& Value, EPropertyOutcome& Outcome)
{
    const FOnlineSessionSettings& Settings = SessionResult.OnlineResult.Session.SessionSettings;
    
    if (const FOnlineSessionSetting* Setting = Settings.Settings.Find(Key))
    {
        Value = Setting->Data.ToString();
        
        if (Value.IsEmpty())
        {
            Outcome = EPropertyOutcome::Empty;
        }
        else
        {
            Outcome = EPropertyOutcome::Valid;
        }
        return;
    }
    
    Value = TEXT("");
    Outcome = EPropertyOutcome::Invalid;
}

void UAdvancedSessionSubsystem::RestartSteamSubsystem()
{
    if (SteamAPI_RestartAppIfNecessary(480))
    {
        FGenericPlatformMisc::RequestExit(false);
    }
}

void UAdvancedSessionSubsystem::GetCurrentSession(FBlueprintSessionResult& SessionResult, ESessionValidOutcome& Outcome)
{
    const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM);
    if (!Subsystem)
    {
        Outcome = ESessionValidOutcome::Invalid;
        return;
    }

    if (const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface(); SessionInterface.IsValid())
    {
        if (FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession))
        {
            SessionResult.OnlineResult.Session = CurrentSession->SessionSettings;
            
            SessionResult.OnlineResult.Session.OwningUserId = CurrentSession->OwningUserId;
            SessionResult.OnlineResult.Session.OwningUserName = CurrentSession->OwningUserName;
            
            Outcome = ESessionValidOutcome::Valid;
            return;
        }
    }
    
    Outcome = ESessionValidOutcome::Invalid;
}

uint8 UAdvancedSessionSubsystem::GetMaxPlayersFromSession()
{
    if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM))
    {
        if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface(); SessionInterface.IsValid())
        {
            if (FNamedOnlineSession* ActiveSession = SessionInterface->GetNamedSession(NAME_GameSession))
            {
                return ActiveSession->SessionSettings.NumPublicConnections;
            }
        }
    }
    return 1;
}

FText UAdvancedSessionSubsystem::GetSteamName(const APlayerState* PlayerState) const
{
    if (!PlayerState) return FText::FromString("{PLAYERSTATE_NOT_FOUND}");

    if (const FUniqueNetIdRepl UniqueId = PlayerState->GetUniqueId(); UniqueId.IsValid())
    {
        if (const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM))
        {
            if (const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface(); Identity.IsValid())
            {
                return FText::FromString(Identity->GetPlayerNickname(*UniqueId));
            }
        }
    }
    
    return FText::FromString("{USERNAME_NOT_FOUND}");
}

bool UAdvancedSessionSubsystem::IsSteamActive() const
{
    if (const IOnlineSubsystem* SteamSubsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM); SteamSubsystem && SteamSubsystem->GetIdentityInterface().IsValid())
    {
        return true;
    }
    
    return false;
}