#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AdvancedSessionSubsystem.generated.h"

struct FBlueprintSessionResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreationComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionFindComplete, const TArray<FBlueprintSessionResult>&, Results, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionJoinComplete, bool, bWasSuccessful);

UENUM(BlueprintType)
enum class ESessionValidOutcome : uint8
{
	Valid,
	Invalid
};

UENUM(BlueprintType)
enum class EPropertyOutcome : uint8
{
	Valid,
	Invalid,
	Empty
};

UCLASS()
class EVERWRESTLE_API UAdvancedSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Session Logistics
	UFUNCTION(BlueprintCallable, Category="Networking | Session Logistics")
	void CreateGameSession(int32 MaxPlayers = 4, bool bIsLan = false, bool bAllowInvites = true, bool bIsDedicated = false, bool bUseLobbiesIfAvailable = true, bool bUsePresence = true, bool bAllowJoinViaPresence = true, bool bAllowJoinViaPresenceFriendsOnly = false, bool bAntiCheatProtected = false, bool bUsesStats = false, bool bShouldAdvertise = true, bool bUseLobbiesVoiceChatIfAvailable = false, bool bAllowJoinInProgress = true, FString ServerName = "DEFAULT_SERVER_NAME", FString ServerPassword = "");
	
	UFUNCTION(BlueprintCallable, Category="Networking | Session Logistics")
	void JoinGameSession(const FBlueprintSessionResult& SessionToJoin);
	
	UFUNCTION(BlueprintCallable, Category="Networking | Session Logistics")
	void FindGameSessions(const int32 MaxResults, bool bIsLan = false);
	
	UFUNCTION(BlueprintCallable, Category="Networking | Session Logistics", meta = (ExpandEnumAsExecs="Outcome"))
	void GetCurrentSession(FBlueprintSessionResult& SessionResult, ESessionValidOutcome& Outcome);
	
	UFUNCTION(BlueprintCallable, Category="Networking | Session Logistics")
	uint8 GetMaxPlayersFromSession();

	UFUNCTION(BlueprintCallable, Category="Networking | Session Logistics", meta = (ExpandEnumAsExecs="Outcome"))
	void GetSessionProperty(const FBlueprintSessionResult& SessionResult, const FName Key, FString& Value, EPropertyOutcome& Outcome);

	// Steam Services
	UFUNCTION(BlueprintCallable, Category="Networking | Steam Services")
	void RestartSteamSubsystem();
	UFUNCTION(BlueprintCallable, Category="Networking | Steam Services")
	FText GetSteamName(const APlayerState* PlayerState) const;
	
	UFUNCTION(BlueprintCallable, Category="Networking | Steam Services")
	bool IsSteamActive() const;
	
	// Networking Delegates
	UPROPERTY(BlueprintAssignable, Category="Networking | Delegates")
	FOnSessionCreationComplete OnSessionCreationComplete;
	
	UPROPERTY(BlueprintAssignable, Category="Networking | Delegates")
	FOnSessionJoinComplete OnSessionJoinComplete;
	
	UPROPERTY(BlueprintAssignable, Category="Networking | Delegates")
	FOnSessionFindComplete OnSessionFindComplete;

private:
	// Internal Callbacks
	FDelegateHandle FindSessionsHandle;
	FDelegateHandle CreateSessionsHandle;
	FDelegateHandle JoinSessionsHandle;
	
	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void HandleFindSessionComplete(bool bWasSuccessful);
	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};