// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SExplosiveBarrel.h"
#include "Public/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

    HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

    RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
    RadialForceComp->Radius = 500.0f;
    RadialForceComp->bImpulseVelChange = true;
    RadialForceComp->bIgnoreOwningActor = true;
    RadialForceComp->SetAutoActivate(false);
    RadialForceComp->SetupAttachment(MeshComp);

    SetReplicates(true);
    SetReplicateMovement(true);
}

void ASExplosiveBarrel::OnRep_Died()
{
    MeshComp->SetMaterial(0, ExplosedMat);
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosedEffect, GetActorLocation(), FRotator(1));
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

    if (Role == ROLE_Authority)
    {
        HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
    }
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta,
    const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Health <= 0.0f && !bDied)
    {
        bDied = true;
        OnRep_Died();

        MeshComp->AddImpulse(FVector(0, 0, 500), NAME_None, true);
        RadialForceComp->FireImpulse();
    }
}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASExplosiveBarrel, bDied);
}