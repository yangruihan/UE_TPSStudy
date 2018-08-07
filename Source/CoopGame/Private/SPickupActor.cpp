// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "Public/SPowerupActor.h"
#include "TimerManager.h"
#include "SCharacter.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComp->SetSphereRadius(75.0f);
    RootComponent = SphereComp;

    DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
    DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
    DecalComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

    Respawn();
}

void ASPickupActor::Respawn()
{
    if (PowerupClass == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("PowerupClass is nullptr in %s"), *GetName());
        return;
    }

    FActorSpawnParameters parameters;
    parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass, GetTransform(), parameters);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (PowerupInstance == nullptr)
        return;

    const auto Player = Cast<ASCharacter>(OtherActor);
    if (Player == nullptr)
        return;

    PowerupInstance->Active();
    PowerupInstance = nullptr;

    GetWorldTimerManager().SetTimer(TimerHandle_RespawnHandler, this, &ASPickupActor::Respawn, CooldownDuration);
}
