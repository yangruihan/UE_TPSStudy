// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Public/SHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Public/SCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

// Sets default values
ATrackerBot::ATrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCanEverAffectNavigation(false);
    MeshComp->SetSimulatePhysics(true);
    RootComponent = MeshComp;
    
    HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
    HealthComp->DefaultHealth = 100.0f;

    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComp->SetSphereRadius(180.0f);
    SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SphereComp->SetupAttachment(RootComponent);
    
    bUseVelocityChagne = true;
    ReachRequiredDistance = 100.0f;
    MovementForce = 1000.0f;

    bExplosion = false;
    ExplosionDamage = 40.0f;
    ExplosionRange = 200.0f;

    bStartSelfDamage = false;

    SelfDamageInterval = 0.5f;
}

// Called when the game starts or when spawned
void ATrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
    HealthComp->OnHealthChanged.AddDynamic(this, &ATrackerBot::OnHealthChanged);
    
    NextPathPoint = GetNextPathPoint();
}

FVector ATrackerBot::GetNextPathPoint()
{
    auto PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (PlayerPawn)
    {
        auto NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
        if (NavPath && NavPath->PathPoints.Num() > 1)
        {
            return NavPath->PathPoints[1];
        }
    }
    
    return GetActorLocation();
}

// Called every frame
void ATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    auto Vector = NextPathPoint - GetActorLocation();
    auto Distance = Vector.Size();
    if (Distance <= ReachRequiredDistance)
    {
        NextPathPoint = GetNextPathPoint();
    }
    else
    {
        auto ForceDirection = Vector;
        ForceDirection.Normalize();
        ForceDirection *= MovementForce;
        MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChagne);
    }
    
    DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
}

void ATrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
    if (!bStartSelfDamage && !bExplosion)
    {
        auto PlayerPawn = Cast<ASCharacter>(OtherActor);
        if (PlayerPawn)
        {
            GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ATrackerBot::SelfDamage, SelfDamageInterval, true, 0);

            if (SelfDestructSoundEffect)
            {
                UGameplayStatics::SpawnSoundAttached(SelfDestructSoundEffect, RootComponent);
            }
        }
    }
}

void ATrackerBot::SelfDestruct()
{
    if (bExplosion)
        return;

    bExplosion = true;

    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
    }

    TArray<AActor*> IgnoreActor;
    IgnoreActor.Add(this);

    UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRange, nullptr, IgnoreActor, this, GetInstigatorController(), true);

    if (ExplosionSoundEffect)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSoundEffect, GetActorLocation());
    }

    DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRange, 12, FColor::Red, false, 2);

    Destroy();
}

void ATrackerBot::SelfDamage()
{
    UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), this, nullptr);
}

void ATrackerBot::OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta,
                                  const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (!MatInstance)
    {
        MatInstance = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
    }
    
    if (MatInstance)
    {
        MatInstance->SetScalarParameterValue("LastTimeTakeDamaged", GetWorld()->TimeSeconds);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (%s)"), *FString::SanitizeFloat(Health), *GetName());

    if (Health <= 0.0f)
    {
        SelfDestruct();
    }
}
