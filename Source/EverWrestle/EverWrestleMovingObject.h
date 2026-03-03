// EverWrestle - A Peak Entertainment Production (2026)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "EverWrestleMovingObject.generated.h"

UENUM()
enum class EMovementDirection : uint8
{
	Forward,
	Backward
};

UCLASS()
class EVERWRESTLE_API AEverWrestleMovingObject : public AActor
{
	GENERATED_BODY()
	
public:
	AEverWrestleMovingObject();

protected:
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	// Internal Physics & Animation
	UFUNCTION()
	void OnTimelineUpdate(float Alpha);

	UFUNCTION()
	void OnTimelineFinished();

	UFUNCTION()
	void OnRep_MoveData();

	void StartMovement_Server();
	void StartMovement_Local();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InGame | Physics & Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "InGame | Physics & Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> MoveTimeline;
	
	// Configuration
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InGame | Physics & Animation", meta = (AllowPrivateAccess = "true", MakeEditWidget))
	FVector TargetRelativeLocation;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InGame | Physics & Animation", meta = (AllowPrivateAccess = "true", Units = "s"))
	float MoveDuration = 2.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InGame | Physics & Animation", meta = (AllowPrivateAccess = "true", Units = "s"))
	float PauseDuration = 1.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InGame | Physics & Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> MoveCurve;
	
	// Replicated State
	UPROPERTY(ReplicatedUsing = OnRep_MoveData)
	FVector ReplicatedStartLocation;

	UPROPERTY(ReplicatedUsing = OnRep_MoveData)
	FVector ReplicatedTargetLocation;

	FVector InitialWorldLocation;
	FVector TargetWorldLocation;
	FVector LocalStartLocation;
	FVector LocalTargetLocation;
	EMovementDirection MovementDirection = EMovementDirection::Forward;
	FTimerHandle PauseTimerHandle;
};