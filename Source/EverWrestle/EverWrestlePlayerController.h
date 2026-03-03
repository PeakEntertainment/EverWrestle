// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EverWrestlePlayerController.generated.h"

class UInputMappingContext;

UCLASS(abstract)
class AEverWrestlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void ClientRestart_Implementation(class APawn* NewPawn) override;

	// Player Input Configuration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="InGame | Player Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;
	
private:
	// Internal Logistics
	void ResetInputComponent();
};