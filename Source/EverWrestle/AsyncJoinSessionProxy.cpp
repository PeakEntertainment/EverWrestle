#include "AsyncJoinSessionProxy.h"

#include "AdvancedSessionSubsystem.h"

UAsyncJoinSessionProxy* UAsyncJoinSessionProxy::JoinSessionAsync(UObject* WorldContextObject, const FBlueprintSessionResult& SessionToJoin)
{
	UAsyncJoinSessionProxy* Proxy = NewObject<UAsyncJoinSessionProxy>();
	Proxy->WorldContext = WorldContextObject;
	Proxy->InternalSessionToJoin = SessionToJoin;
	Proxy->RegisterWithGameInstance(WorldContextObject);
	return Proxy;
}

void UAsyncJoinSessionProxy::Activate()
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
		SessionSubsystem->OnSessionJoinComplete.AddUniqueDynamic(this, &UAsyncJoinSessionProxy::HandleSubsystemCallback);
		
		SessionSubsystem->JoinGameSession(InternalSessionToJoin);
	}
	else
	{
		OnFailure.Broadcast();
	}
}

void UAsyncJoinSessionProxy::HandleSubsystemCallback(bool bWasSuccessful)
{
	if (WorldContext.IsValid())
	{
		if (UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
		{
			if (UAdvancedSessionSubsystem* SessionSubsystem = GI->GetSubsystem<UAdvancedSessionSubsystem>())
			{
				SessionSubsystem->OnSessionJoinComplete.RemoveDynamic(this, &UAsyncJoinSessionProxy::HandleSubsystemCallback);
			}
		}
	}
	
	if (bWasSuccessful)
		OnSuccess.Broadcast();
	else
		OnFailure.Broadcast();

	SetReadyToDestroy();
}
