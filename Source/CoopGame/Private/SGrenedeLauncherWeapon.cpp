// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SGrenedeLauncherWeapon.h"
#include "Components/SkeletalMeshComponent.h"


void ASGrenedeLauncherWeapon::Fire()
{
    auto Owner = GetOwner();
    if (Owner && ProjectileClass)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        FVector MuzzlePosition = MeshComp->GetSocketLocation(MuzzleSocketName);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzlePosition, EyeRotation, SpawnParams);
    }
}
