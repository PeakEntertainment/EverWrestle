// EverWrestle - A Peak Entertainment Production (2026)

#include "EverWrestleObjectSpawner.h"

#include "EverWrestleMacros.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AEverWrestleObjectSpawner::AEverWrestleObjectSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    RootComponent = BoxCollider;

    Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
    Arrow->SetupAttachment(BoxCollider);
    Arrow->SetUsingAbsoluteScale(true);
    Arrow->SetArrowSize(5.f);
    Arrow->SetArrowLength(100.f);
}

void AEverWrestleObjectSpawner::BeginPlay()
{
    Super::BeginPlay();
    Server_StartSpawnCycle();
}

void AEverWrestleObjectSpawner::Server_StartSpawnCycle_Implementation()
{
    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &AEverWrestleObjectSpawner::SpawnObjects,
        SpawnInterval,
        true
    );
}

void AEverWrestleObjectSpawner::SpawnObjects()
{
    if (ObjectsToSpawn.IsEmpty() || !GetWorld()) return;

    const int32 NumToSpawn = FMath::RandRange(MinObjectSpawn, MaxObjectSpawn);

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

    for (int32 i = 0; i < NumToSpawn; ++i)
    {
        const int32 Index = FMath::RandRange(0, ObjectsToSpawn.Num() - 1);
        TSubclassOf<AActor> ClassToSpawn = ObjectsToSpawn[Index];

        if (!ClassToSpawn) continue;

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(GetRandomSpawnLocationInsideCube());
        SpawnTransform.SetRotation(GetRandomRotation().Quaternion());

        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTransform, SpawnParams);

        if (!SpawnedActor) continue;

        SpawnedActor->SetActorScale3D(GetRandomScale(MinObjectScale, MaxObjectScale));

        if (UPrimitiveComponent* PrimComp = SpawnedActor->FindComponentByClass<UPrimitiveComponent>())
        {
            if (PrimComp->IsSimulatingPhysics())
            {
                const FVector Impulse = Arrow->GetForwardVector() * ImpulseStrength;
                PrimComp->AddImpulse(Impulse, NAME_None, true);
            }
        }

        if (SpawnedActor->GetClass()->ImplementsInterface(UInitiateSelfDestructInterface::StaticClass()))
        {
            IInitiateSelfDestructInterface::Execute_InitiateSelfDestruct(SpawnedActor, ObjectDespawnDuration);
        }
    }
}

FVector AEverWrestleObjectSpawner::GetRandomSpawnLocationInsideCube() const
{
    const FVector Extent = BoxCollider->GetScaledBoxExtent();

    const float X = FMath::RandRange(-Extent.X, Extent.X);
    const float Y = FMath::RandRange(-Extent.Y, Extent.Y);
    const float Z = FMath::RandRange(-Extent.Z, Extent.Z);

    return BoxCollider->GetComponentLocation() + FVector(X, Y, Z);
}

FRotator AEverWrestleObjectSpawner::GetRandomRotation() const
{
    return FRotator(
        FMath::RandRange(0.f, 360.f),
        FMath::RandRange(0.f, 360.f),
        FMath::RandRange(0.f, 360.f)
    );
}

FVector AEverWrestleObjectSpawner::GetRandomScale(float InMinScale, float InMaxScale) const
{
    return FVector(
        FMath::RandRange(InMinScale, InMaxScale),
        FMath::RandRange(InMinScale, InMaxScale),
        FMath::RandRange(InMinScale, InMaxScale)
    );
}