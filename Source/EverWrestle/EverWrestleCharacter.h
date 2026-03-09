#pragma once

#include "CoreMinimal.h"
#include "EverWrestleCharacterInterface.h"
#include "GameFramework/Character.h"
#include "EverWrestleCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(abstract)
class AEverWrestleCharacter : public ACharacter, public IEverWrestleCharacterInterface
{
	GENERATED_BODY()

public:
	AEverWrestleCharacter();

	// World Interaction
	UFUNCTION(BlueprintCallable, Category="InGame | World Interaction")
	void SetCheckpoint(const FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "InGame | World Interaction")
	virtual void SetPlayerCheckpoint_Implementation(FVector CheckpointPosition) override;

	virtual void ResetToLastCheckpoint_Implementation() override;
	
	// Player Appearance
	void UpdateCharacterMesh();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Player Input
	UFUNCTION(BlueprintCallable, Category="InGame | Player Input")
	void DoMove(FVector2D MovementDirection);

	UFUNCTION(BlueprintCallable, Category="InGame | Player Input")
	void DoLook(FVector2D LookDirection);

	UFUNCTION(BlueprintCallable, Category="InGame | Player Input")
	void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="InGame | Player Input")
	void DoJumpEnd();

	// Combat & Health
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "InGame | Combat & Health")
	void RequestPointsForKnockout();
	
	UFUNCTION(BlueprintCallable, Category = "InGame | Combat & Health")
	void KnockoutInitiated();

private:
	// World Interaction Internal
	UFUNCTION(Server, Reliable, Category = "InGame | World Interaction")
	void Server_ResetToLastCheckpoint();
	
	// Internal Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(Replicated)
	FVector LastCheckPointLocation;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "InGame | Combat & Health", meta = (AllowPrivateAccess = "true"))
	uint8 CurrentKnockouts = 0;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};