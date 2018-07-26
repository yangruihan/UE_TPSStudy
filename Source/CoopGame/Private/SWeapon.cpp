// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Engine/World.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing (
    TEXT("COOP.DebugWeapons"),
    DebugWeaponDrawing,
    TEXT("Draw Debug Lines for weapons"),
    ECVF_Cheat
);

// Sets default values
ASWeapon::ASWeapon()
{
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    MuzzleSocketName = "MuzzleSocket";
    TracerTargetParamName = "Target";

    BaseDamage = 20.0f;
    CritDamageRate = 5.0f;

    RPM = 600;

    SetReplicates(true);
}

void ASWeapon::BeginPlay()
{
    Super::BeginPlay();

    TimeBetweenShots = 60.0f / RPM;
}

void ASWeapon::PlayFireEffect(FVector TracerEndPoint)
{
    if (MuzzleEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
    }

    if (TracerEffect)
    {
        FVector TracerLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

        auto TracerEffectComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, TracerLocation);
        if (TracerEffectComp)
        {
            TracerEffectComp->SetVectorParameter(TracerTargetParamName, TracerEndPoint);
        }
    }

    auto Owner =Cast<APawn>(GetOwner());
    if (Owner)
    {
        auto PC = Cast<APlayerController>(Owner->GetController());
        if (PC)
        {
            PC->ClientPlayCameraShake(CameraShake);
        }
    }
}

void ASWeapon::Fire()
{
    if (Role < ROLE_Authority)
    {
        ServerFire();
    }

    auto Owner = GetOwner();
    if (Owner)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        FVector EyeDirection = EyeRotation.Vector();

        FVector TraceEndPos = EyeLocation + EyeDirection * 10000;
        FVector TracerEffectTargetPos = TraceEndPos;

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Owner);
        Params.AddIgnoredActor(this);
        Params.bTraceComplex = true;
        Params.bReturnPhysicalMaterial = true;

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEndPos, COLLISION_WEAPON, Params))
        {

            EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
            UParticleSystem* SelectedEffect = nullptr;

            float ActualDamage = BaseDamage;

            switch (SurfaceType)
            {
            case SURFACE_FLESH_DEFAULT:
                SelectedEffect = FleshImpactEffect;
                break;

            case SURFACE_FLESH_VULNERABLE:
                SelectedEffect = FleshImpactEffect;
                ActualDamage *= CritDamageRate;
                break;

            default:
                SelectedEffect = DefaultImpactEffect;
                break;
            }

            UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), ActualDamage, EyeDirection, HitResult, Owner->GetInstigatorController(), this, DamageType);

            if (SelectedEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
            }

            TracerEffectTargetPos = HitResult.ImpactPoint;
        }

        PlayFireEffect(TracerEffectTargetPos);

        LastShotTime = GetWorld()->GetTimeSeconds();

        if (DebugWeaponDrawing > 0)
        {
            DrawDebugLine(GetWorld(), EyeLocation, TraceEndPos, FColor::White, false, 0.5, 0, 1);
        }
    }
}

void ASWeapon::ServerFire_Implementation()
{
    Fire();
}

bool ASWeapon::ServerFire_Validate()
{
    return true;
}

void ASWeapon::StartFire()
{
    float DelayTime = FMath::Max(LastShotTime + TimeBetweenShots - GetWorld()->GetTimeSeconds(), 0.0f);
    GetWorldTimerManager().SetTimer(TimeHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, DelayTime);
}

void ASWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(TimeHandle_TimeBetweenShots);
}
