#include "AsyncCreateSessionProxy.h"

#include "AdvancedSessionSubsystem.h"

UAsyncCreateSessionProxy* UAsyncCreateSessionProxy::CreateSessionAsync(UObject* WorldContextObject, int32 MaxPlayers,
                                                                       bool bIsLan, bool bAllowInvites, bool bIsDedicated, bool bUseLobbiesIfAvailable, bool bUsePresence,
                                                                       bool bAllowJoinViaPresence, bool bAllowJoinViaPresenceFriendsOnly, bool bAntiCheatProtected, bool bUsesStats,
                                                                       bool bShouldAdvertise, bool bUseLobbiesVoiceChatIfAvailable, bool bAllowJoinInProgress, FString ServerName,
                                                                       FString ServerPassword)
{
	UAsyncCreateSessionProxy* Proxy = NewObject<UAsyncCreateSessionProxy>();
	Proxy->WorldContext = WorldContextObject;
	Proxy->InternalMaxPlayers = MaxPlayers;
	Proxy->InternalIsLan = bIsLan;
	Proxy->InternalAllowInvites = bAllowInvites;
	Proxy->InternalIsDedicated = bIsDedicated;
	Proxy->InternalUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
	Proxy->InternalUsePresence = bUsePresence;
	Proxy->InternalAllowJoinViaPresence = bAllowJoinViaPresence;
	Proxy->InternalAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	Proxy->InternalAntiCheatProtected = bAntiCheatProtected;
	Proxy->InternalUsesStats = bUsesStats;
	Proxy->InternalShouldAdvertise = bShouldAdvertise;
	Proxy->InternalUseLobbiesVoiceChatIfAvailable = bUseLobbiesVoiceChatIfAvailable;
	Proxy->InternalAllowJoinInProgress = bAllowJoinInProgress;
	Proxy->InternalServerName = ServerName;
	Proxy->InternalServerPassword = ServerPassword;
	Proxy->RegisterWithGameInstance(WorldContextObject);
	return Proxy;
}

void UAsyncCreateSessionProxy::Activate()
{
	if (UWorld* World = WorldContext->GetWorld(); !WorldContext.IsValid() || !IsValid(World))
	{
		OnFailure.Broadcast();
		SetReadyToDestroy();
		return;
	}

	const UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance();

	if (UAdvancedSessionSubsystem* SessionSubsystem = GI->GetSubsystem<UAdvancedSessionSubsystem>())
	{
		SessionSubsystem->OnSessionCreationComplete.AddUniqueDynamic(this, &UAsyncCreateSessionProxy::HandleSystemCallback);
		
		SessionSubsystem->CreateGameSession(InternalMaxPlayers, InternalIsLan, InternalAllowInvites, InternalIsDedicated, InternalUseLobbiesIfAvailable, InternalUsePresence, InternalAllowJoinViaPresence, InternalAllowJoinViaPresenceFriendsOnly, InternalAntiCheatProtected, InternalUsesStats, InternalShouldAdvertise, InternalUseLobbiesVoiceChatIfAvailable, InternalAllowJoinInProgress, InternalServerName, InternalServerPassword);
	}
	else
	{
		OnFailure.Broadcast();
	}
}

void UAsyncCreateSessionProxy::HandleSystemCallback(bool bWasSuccessful)
{
	if (WorldContext.IsValid())
	{
		if (const UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
		{
			if (UAdvancedSessionSubsystem* SessionSubsystem = GI->GetSubsystem<UAdvancedSessionSubsystem>())
			{
				SessionSubsystem->OnSessionCreationComplete.RemoveDynamic(this, &UAsyncCreateSessionProxy::HandleSystemCallback);
			}
		}
	}
	
	if (bWasSuccessful)
		OnSuccess.Broadcast();
	else
		OnFailure.Broadcast();
	
	SetReadyToDestroy();
}