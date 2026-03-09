#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncFindSessionsProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindSessionsAsyncOutput, const TArray<FBlueprintSessionResult>&, Results);

UCLASS()
class EVERWRESTLE_API UAsyncFindSessionsProxy : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	// Session Logistics
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternaluseOnly = "true", WorldContext = "WorldContextObject"), Category="Networking | Session Logistics")
	static UAsyncFindSessionsProxy* FindSessionsAsync(UObject* WorldContextObject, int32 MaxResults, bool bIsLan);
	
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "Networking | Delegates")
	FOnFindSessionsAsyncOutput OnSuccess;
	
	UPROPERTY(BlueprintAssignable, Category = "Networking | Delegates")
	FOnFindSessionsAsyncOutput OnFailure;
	
private:
	// Internal Logistics
	UFUNCTION()
	void HandleSubsystemCallback(const TArray<FBlueprintSessionResult>& Results, bool bWasSuccessful);
	
	TWeakObjectPtr<UObject> WorldContext;
	int32 InternalMaxResults;
	bool InternalIsLan;
};