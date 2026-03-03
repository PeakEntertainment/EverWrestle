// EverWrestle - A Peak Entertainment Production (2025)


#include "Checkpoint.h"

#include "EverWrestleCharacter.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACheckpoint::Server_SetPlayerCheckPoint_Implementation(AActor* Player) const
{
	if (Player && Player->GetClass()->ImplementsInterface(UEverWrestleCharacterInterface::StaticClass()))
	{
		IEverWrestleCharacterInterface::Execute_SetPlayerCheckpoint(Player, GetActorLocation());
	}
}