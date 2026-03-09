#include "AsyncFindSessionsProxy.h"

#include "AdvancedSessionSubsystem.h"

UAsyncFindSessionsProxy* UAsyncFindSessionsProxy::FindSessionsAsync(UObject* WorldContextObject, int32 MaxResults,
                                                                    bool bIsLan)
{
	UAsyncFindSessionsProxy* Proxy = NewObject<UAsyncFindSessionsProxy>();
	Proxy->WorldContext = WorldContextObject;
	Proxy->InternalMaxResults = MaxResults;
	Proxy->InternalIsLan = bIsLan;
	Proxy->RegisterWithGameInstance(WorldContextObject);
	return Proxy;
}

void UAsyncFindSessionsProxy::Activate()
{
	if (UWorld* World = WorldContext->GetWorld(); !WorldContext.IsValid() || !IsValid(World))
	{
		OnFailure.Broadcast({}); 
		SetReadyToDestroy();
		return;
	}

	const UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance();

	if (UAdvancedSessionSubsystem* SessionSubsystem = GI->GetSubsystem<UAdvancedSessionSubsystem>())
	{
		SessionSubsystem->OnSessionFindComplete.AddUniqueDynamic(this, &UAsyncFindSessionsProxy::HandleSubsystemCallback);
		
		SessionSubsystem->FindGameSessions(InternalMaxResults, InternalIsLan);
	}
	else
	{
		OnFailure.Broadcast({});
	}
}

void UAsyncFindSessionsProxy::HandleSubsystemCallback(const TArray<FBlueprintSessionResult>& Results, bool bWasSuccessful)
{
	if (WorldContext.IsValid())
	{
		if (const UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
		{
			if (UAdvancedSessionSubsystem* SessionSubsystem = GI->GetSubsystem<UAdvancedSessionSubsystem>())
			{
				SessionSubsystem->OnSessionFindComplete.RemoveDynamic(this, &UAsyncFindSessionsProxy::HandleSubsystemCallback);
			}
		}
	}
	
	if (bWasSuccessful)
		OnSuccess.Broadcast(Results);
	else
		OnFailure.Broadcast({});
	
	SetReadyToDestroy();
}
