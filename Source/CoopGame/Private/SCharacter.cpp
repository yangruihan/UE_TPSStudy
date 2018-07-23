// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->SetupAttachment(RootComponent);

    GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp);

    bWantsToZoom = false;
    ZoomSpeed = 20.0f;
    ZoomedFov = 60.0f;

    RunSpeed = 1500.0f;

    WeaponSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

    DefaultFOV = CameraComp->FieldOfView;

    MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
    DefaultSpeed = MoveComp->MaxWalkSpeed;

    // Set default weapon
    if (DefaultWeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(DefaultWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

        if (CurrentWeapon)
        {
            CurrentWeapon->SetOwner(this);
            CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
        }
    }
}

void ASCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginRun()
{
    MoveComp->MaxWalkSpeed = RunSpeed;
}

void ASCharacter::EndRun()
{
    MoveComp->MaxWalkSpeed = DefaultSpeed;
}

void ASCharacter::BeginCrouch()
{
    Crouch();
}

void ASCharacter::EndCrouch()
{
    UnCrouch();
}

void ASCharacter::BeginZoom()
{
    bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
    bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StartFire();
    }
}

void ASCharacter::StopFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
    }
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

    PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);
 
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

    PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
    PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASCharacter::BeginRun);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASCharacter::EndRun);
}

FVector ASCharacter::GetPawnViewLocation() const
{
    if (CameraComp)
    {
        return CameraComp->GetComponentLocation();
    }

    return Super::GetPawnViewLocation();
}

void ASCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    float TargetFOV = bWantsToZoom ? ZoomedFov : DefaultFOV;
    float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaSeconds, ZoomSpeed);

    CameraComp->SetFieldOfView(NewFOV);
}

