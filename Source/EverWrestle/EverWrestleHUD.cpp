// EverWrestle - A Peak Entertainment Production (2025)


#include "EverWrestleHUD.h"

#include "Blueprint/UserWidget.h"

void AEverWrestleHUD::SetCurrentWidget(const TSubclassOf<UUserWidget> Widget, const EInputType InputType,
	const bool bShouldShowCursor)
{
	if (APlayerController* PC = GetOwningPlayerController(); PC && Widget)
	{
		if (CurrentGameHUD) CurrentGameHUD->RemoveFromParent();
		
		CurrentGameHUD = CreateWidget<UUserWidget>(PC, Widget);
		
		if (CurrentGameHUD)
		{
			CurrentGameHUD->AddToViewport();
			
			PC->bShowMouseCursor = bShouldShowCursor;

			switch (InputType)
			{
			case EInputType::UIOnly:
				{
					FInputModeUIOnly InputModeUI;
					InputModeUI.SetWidgetToFocus(CurrentGameHUD->TakeWidget());
					InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					PC->SetInputMode(InputModeUI);
				}
				break;
			case EInputType::GameOnly:
				{
					const FInputModeGameOnly InputModeGame;
					PC->SetInputMode(InputModeGame);
				}
				break;
			case EInputType::GameAndUI:
				{
					FInputModeGameAndUI InputModeGameUI;
					InputModeGameUI.SetWidgetToFocus(CurrentGameHUD->TakeWidget());
					InputModeGameUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					PC->SetInputMode(InputModeGameUI);
				}
				break;
			default:
				{
					const FInputModeGameAndUI InputModeDefault;
					PC->SetInputMode(InputModeDefault);
				}
				break;
			}
		}
	}
}
