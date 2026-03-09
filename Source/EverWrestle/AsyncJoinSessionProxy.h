#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncJoinSessionProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinSessionAsyncOutput);

UCLASS()
class EVERWRESTLE_API UAsyncJoinSessionProxy : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	// Session Logistics
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Networking | Session Logistics")
	static UAsyncJoinSessionProxy* JoinSessionAsync(UObject* WorldContextObject, const FBlueprintSessionResult& SessionToJoin);
	
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "Networking | Delegates")
	FOnJoinSessionAsyncOutput OnSuccess;
	
	UPROPERTY(BlueprintAssignable, Category = "Networking | Delegates")
	FOnJoinSessionAsyncOutput OnFailure;
	
private:
	// Internal Logistics
	UFUNCTION()
	void HandleSubsystemCallback(bool bWasSuccessful);
	
	TWeakObjectPtr<UObject> WorldContext;
	FBlueprintSessionResult InternalSessionToJoin;
};