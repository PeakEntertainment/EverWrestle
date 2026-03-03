// EverWrestle - A Peak Entertainment Production (2025)

#pragma once

#include "CoreMinimal.h"
#include "RoundEndWidget.h"
#include "GameFramework/HUD.h"
#include "EverWrestleHUD.generated.h"

UENUM(BlueprintType)
enum class EInputType : uint8
{
	UIOnly		UMETA(DisplayName = "UI Only"),
	GameOnly	UMETA(DisplayName = "Game Only"),
	GameAndUI	UMETA(DisplayName = "Game and UI Only")
};

UCLASS()
class EVERWRESTLE_API AEverWrestleHUD : public AHUD
{
	GENERATED_BODY()
public:
	// UI Management
	UFUNCTION(BlueprintCallable, Category = "InGame | UI Management")
	void SetCurrentWidget(const TSubclassOf<UUserWidget> Widget, const EInputType InputType, bool bShouldShowCursor);
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "InGame | UI Management")
	TObjectPtr<UUserWidget> CurrentGameHUD;
};