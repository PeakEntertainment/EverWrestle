#pragma once

#include "CoreMinimal.h"
#include "AdvancedSessionSubsystem.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncCreateSessionProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateSessionAsyncOutput);

UCLASS()
class EVERWRESTLE_API UAsyncCreateSessionProxy : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	// Session Logistics
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Networking | Session Logistics")
	static UAsyncCreateSessionProxy* CreateSessionAsync(UObject* WorldContextObject, int32 MaxPlayers = 4, bool bIsLan = false, bool bAllowInvites = true, bool bIsDedicated = false, bool bUseLobbiesIfAvailable = true, bool bUsePresence = true, bool bAllowJoinViaPresence = true, bool bAllowJoinViaPresenceFriendsOnly = false, bool bAntiCheatProtected = false, bool bUsesStats = false, bool bShouldAdvertise = true, bool bUseLobbiesVoiceChatIfAvailable = false, bool bAllowJoinInProgress = true, FString ServerName = "DEFAULT_SERVER_NAME", FString ServerPassword = "");
	
	virtual void Activate() override;
	
	UPROPERTY(BlueprintAssignable, Category = "Networking | Delegates")
	FOnCreateSessionAsyncOutput OnSuccess;
	
	UPROPERTY(BlueprintAssignable, Category = "Networking | Delegates")
	FOnCreateSessionAsyncOutput OnFailure;
	
private:
	// Internal Logistics
	UFUNCTION()
	void HandleSystemCallback(bool bWasSuccessful);
	
	TWeakObjectPtr<UObject> WorldContext;
	int32 InternalMaxPlayers;
	bool InternalIsLan;
	bool InternalAllowInvites;
	bool InternalIsDedicated;
	bool InternalUseLobbiesIfAvailable;
	bool InternalUsePresence;
	bool InternalAllowJoinViaPresence;
	bool InternalAllowJoinViaPresenceFriendsOnly;
	bool InternalAntiCheatProtected;
	bool InternalUsesStats;
	bool InternalShouldAdvertise;
	bool InternalUseLobbiesVoiceChatIfAvailable;
	bool InternalAllowJoinInProgress;
	FString InternalServerName;
	FString InternalServerPassword;
};