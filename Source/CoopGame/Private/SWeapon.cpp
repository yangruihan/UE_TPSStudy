// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

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
        if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEndPos, ECC_Visibility, Params))
        {
            UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), 20.0f, EyeDirection, HitResult, Owner->GetInstigatorController(), this, DamageType);

            EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
            UParticleSystem* SelectedEffect = nullptr;

            switch (SurfaceType)
            {
            case SURFACE_FLESH_DEFAULT:
            case SURFACE_FLESH_VULNERABLE:
                SelectedEffect = FleshImpactEffect;
                break;

            default:
                SelectedEffect = DefaultImpactEffect;
                break;
            }

            if (SelectedEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
            }

            TracerEffectTargetPos = HitResult.ImpactPoint;
        }

        PlayFireEffect(TraceEndPos);

        if (DebugWeaponDrawing > 0)
        {
            DrawDebugLine(GetWorld(), EyeLocation, TraceEndPos, FColor::White, false, 0.5, 0, 1);
        }
    }
}
