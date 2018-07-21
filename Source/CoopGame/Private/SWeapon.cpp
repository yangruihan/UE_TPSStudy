// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASWeapon::ASWeapon()
{
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
    auto Owner = GetOwner();
    if (Owner)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        FVector TraceEndPos = EyeLocation + (EyeRotation.Vector() * 10000);

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Owner);
        Params.AddIgnoredActor(this);
        Params.bTraceComplex = true;

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEndPos, ECC_Visibility, Params))
        {
            // hit

        }

        DrawDebugLine(GetWorld(), EyeLocation, TraceEndPos, FColor::White, false, 0.5, 0, 1);
    }
}
