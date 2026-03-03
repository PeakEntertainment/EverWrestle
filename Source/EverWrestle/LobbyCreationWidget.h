// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyCreationWidget.generated.h"

class UAdvancedSessionSubsystem;
class UEditableTextBox;

UCLASS()
class EVERWRESTLE_API ULobbyCreationWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	// UI Management
	UFUNCTION(BlueprintCallable, Category = "InGame | UI Management")
	void CheckForNameLength(const FText& ChangedText, const uint8 MaxLength, UEditableTextBox* InputFieldToCheck);

	// Session Configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking | Session Logistics", meta = (AllowPrivateAccess = "true"))
	uint8 MaxSessionNameLength = 15;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking | Session Logistics", meta = (AllowPrivateAccess = "true"))
	uint8 MaxPasswordLength = 15;

	UPROPERTY()
	TObjectPtr<UAdvancedSessionSubsystem> SessionSubsystem;
};