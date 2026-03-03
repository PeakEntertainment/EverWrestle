#include "EverWrestleCharacter.h"

#include "EverWrestleGameMode.h"
#include "EverWrestlePlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

AEverWrestleCharacter::AEverWrestleCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AEverWrestleCharacter::SetCheckpoint(const FVector& NewLocation)
{
	LastCheckPointLocation = NewLocation;
}

void AEverWrestleCharacter::ResetToLastCheckpoint_Implementation()
{
	IEverWrestleCharacterInterface::ResetToLastCheckpoint_Implementation();
	
	Server_ResetToLastCheckpoint();
}

void AEverWrestleCharacter::SetPlayerCheckpoint_Implementation(FVector CheckpointPosition)
{
	IEverWrestleCharacterInterface::SetPlayerCheckpoint_Implementation(CheckpointPosition);
	
	SetCheckpoint(CheckpointPosition);
}

void AEverWrestleCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	UpdateCharacterMesh();
}

void AEverWrestleCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	UpdateCharacterMesh();
}

void AEverWrestleCharacter::UpdateCharacterMesh()
{
	if (AEverWrestlePlayerState* PS = GetPlayerState<AEverWrestlePlayerState>())
	{
		PS->ApplyCharacterMesh();
	}
}

void AEverWrestleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	LastCheckPointLocation = GetActorLocation();
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void AEverWrestleCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AEverWrestleCharacter, LastCheckPointLocation);
}

void AEverWrestleCharacter::DoMove(FVector2D MovementDirection)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementDirection.Y);
		AddMovementInput(RightDirection, MovementDirection.X);
	}
}

void AEverWrestleCharacter::DoLook(FVector2D LookDirection)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(LookDirection.X);
		AddControllerPitchInput(LookDirection.Y);
	}
}

void AEverWrestleCharacter::DoJumpStart()
{
	Jump();
}

void AEverWrestleCharacter::DoJumpEnd()
{
	StopJumping();
}

void AEverWrestleCharacter::KnockoutInitiated()
{
	CurrentKnockouts++;
	RequestPointsForKnockout();
}

void AEverWrestleCharacter::Server_ResetToLastCheckpoint_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), false, false);
		SetActorLocation(LastCheckPointLocation, false, nullptr, ETeleportType::ResetPhysics);
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, false);
		GetMesh()->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		GetMesh()->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
}

void AEverWrestleCharacter::RequestPointsForKnockout_Implementation()
{
	APlayerController* PC = GetController<APlayerController>();
	
	if (AEverWrestleGameMode* GM = Cast<AEverWrestleGameMode>(GetWorld()->GetAuthGameMode()); GM && PC)
	{
		GM->AddPointsForKnockout(PC, CurrentKnockouts);
	}
}
