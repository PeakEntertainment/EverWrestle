// EverWrestle - A Peak Entertainment Production (2026)


#include "EverWrestleBearTrap.h"

#include "EverWrestleCharacterInterface.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

AEverWrestleBearTrap::AEverWrestleBearTrap()
{
	PrimaryActorTick.bCanEverTick = false;
	
	TrapMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("TrapMeshComponent");
	TrapCollider = CreateDefaultSubobject<UCapsuleComponent>("TrapCollider");
	
	RootComponent = TrapMeshComponent;
	
	TrapCollider->SetupAttachment(TrapMeshComponent);
}

void AEverWrestleBearTrap::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!HasAuthority()) return;
	
	if (!bHasTrapSnapped)
	{
		if (IsValid(OtherActor) && OtherActor->GetClass()->ImplementsInterface(UEverWrestleCharacterInterface::StaticClass()))
		{
			Server_SnapBearTrap(OtherActor);
		}
	}
}

void AEverWrestleBearTrap::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AEverWrestleBearTrap, bHasTrapSnapped);
}

void AEverWrestleBearTrap::Server_SnapBearTrap_Implementation(AActor* ActorToKnockout)
{
	if (HasAuthority())
	{
		Multicast_SnapBearTrap();
		IEverWrestleCharacterInterface::Execute_LoseConsciousness(ActorToKnockout, KnockedOutDuration);
		GetWorldTimerManager().SetTimer(BearSnapTimerHandle, this, &AEverWrestleBearTrap::ResetTrap, SnapDuration, false);
		bHasTrapSnapped = true;
	}
}

void AEverWrestleBearTrap::Multicast_SnapBearTrap_Implementation()
{
	TrapMeshComponent->PlayAnimation(TrapAnimation, false);
}

void AEverWrestleBearTrap::ResetTrap()
{
	if (!HasAuthority()) return;
	
	Multicast_ResetTrap();
}

void AEverWrestleBearTrap::Multicast_ResetTrap_Implementation()
{
	TrapMeshComponent->PlayAnimation(TrapAnimation, false);
	TrapMeshComponent->SetPlayRate(-1.f);
	TrapMeshComponent->SetPosition(TrapAnimation->GetPlayLength());
	bHasTrapSnapped = false;
}
