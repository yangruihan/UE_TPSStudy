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
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"

// Sets default values
ATrackerBot::ATrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Tags.Add(FName("TrackerBot"));

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

    AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));

    TeammateOverlapSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("TeammateSphereOverlapComp"));
    TeammateOverlapSphereComp->SetSphereRadius(300.0f);
    TeammateOverlapSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TeammateOverlapSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    TeammateOverlapSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TeammateOverlapSphereComp->SetupAttachment(RootComponent);
    
    bUseVelocityChagne = true;
    ReachRequiredDistance = 100.0f;
    MovementForce = 1000.0f;

    bExplosion = false;
    DefaultExplosionDamage = 40.0f;
    DefaultExplosionRange = 200.0f;

    bStartSelfDamage = false;

    SelfDamageInterval = 0.5f;

    MaxPowerValue = 100.0f;
    EachTeammateGivenPower = 20.0f;
    CurrentPowerValue = 0.0f;
}

// Called when the game starts or when spawned
void ATrackerBot::BeginPlay()
{
	Super::BeginPlay();

    if (Role == ROLE_Authority)
    {
        NextPathPoint = GetNextPathPoint();
    }

    ExplosionRange = DefaultExplosionRange;
    ExplosionDamage = DefaultExplosionDamage;

    HealthComp->OnHealthChanged.AddDynamic(this, &ATrackerBot::OnHealthChanged);
    
    SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ATrackerBot::OnMeshCompBeginOverlap);
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

void ATrackerBot::ChangeCurrentPower(int TeammateCount)
{
    if (CurrentPowerValue == TeammateCount * EachTeammateGivenPower)
        return;

    CurrentPowerValue = TeammateCount * EachTeammateGivenPower;

    OnRep_CurrentPowerChanged();
}

void ATrackerBot::OnRep_CurrentPowerChanged()
{
    ExplosionRange = DefaultExplosionRange + (CurrentPowerValue / MaxPowerValue) * 200.0f;
    ExplosionDamage = DefaultExplosionDamage + (CurrentPowerValue / MaxPowerValue) * 40.0f;

    if (!MatInstance)
    {
        MatInstance = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
    }

    if (MatInstance)
    {
        MatInstance->SetScalarParameterValue("PowerLevelAlpha", CurrentPowerValue / MaxPowerValue);
    }
}

// Called every frame
void ATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bExplosion)
        return;

    if (Role == ROLE_Authority)
    {
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

         TArray<UPrimitiveComponent*> OverlappingComps;
         TeammateOverlapSphereComp->GetOverlappingComponents(OverlappingComps);
         auto nearByTeammateCount = 0;
         for (auto i = 0; i < OverlappingComps.Num(); i++)
         {
             auto PrimCop = OverlappingComps[i];
        
             if (PrimCop && PrimCop->GetOwner() != this
                 && PrimCop->GetOwner()->ActorHasTag(FName("TrackerBot")))
             {
                 nearByTeammateCount++;
             }
         }
        
         ChangeCurrentPower(nearByTeammateCount);
    }
    
    DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);

    const auto Vel = GetVelocity().Size();
    const auto FinalSoundValue = UKismetMathLibrary::MapRangeClamped(Vel, 10, 1000, 0.1, 2);
    AudioComp->SetVolumeMultiplier(FinalSoundValue);
}

void ATrackerBot::OnMeshCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if (!bStartSelfDamage && !bExplosion)
    {
        bStartSelfDamage = true;
        
        const auto PlayerPawn = Cast<ASCharacter>(OtherActor);
        if (PlayerPawn)
        {
            DrawDebugString(GetWorld(), GetActorLocation(), FString("Overlap PlayerPawn"), 0, FColor::Red, 2);
            UE_LOG(LogTemp, Log, TEXT("Overlap PlayerPawn (%s, %s)"), *GetName(), *PlayerPawn->GetName());
            
            if (Role == ROLE_Authority)
            {
                GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ATrackerBot::SelfDamage, SelfDamageInterval, true, 0);
            }
            
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

    if (ExplosionSoundEffect)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSoundEffect, GetActorLocation());
    }

    MeshComp->SetVisibility(false, true);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (Role == ROLE_Authority)
    {
        TArray<AActor*> IgnoreActor;
        IgnoreActor.Add(this);

        UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRange, nullptr, IgnoreActor, this, GetInstigatorController(), true);

        SetLifeSpan(2.0f);
    }

    DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRange, 12, FColor::Red, false, 2);
    
    UE_LOG(LogTemp, Log, TEXT("SelfDestruct (%s)"), *GetName());
}

void ATrackerBot::SelfDamage()
{
    UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), this, nullptr);

    DrawDebugString(GetWorld(), GetActorLocation(), FString::SanitizeFloat(HealthComp->GetCurrentHealth()), 0, FColor::Red, 2);
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
        MatInstance->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (%s)"), *FString::SanitizeFloat(Health), *GetName());

    if (Health <= 0.0f)
    {
        SelfDestruct();
    }
}


void ATrackerBot::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATrackerBot, CurrentPowerValue);
}
