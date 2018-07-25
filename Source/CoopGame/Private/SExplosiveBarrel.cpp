// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SExplosiveBarrel.h"
#include "Public/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

    HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

    HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta,
    const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
}