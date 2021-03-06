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
#include "CoopGame.h"
#include "Components/CapsuleComponent.h"
#include "Public/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

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

    GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

    HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

    bWantsToZoom = false;
    ZoomSpeed = 20.0f;
    ZoomedFov = 60.0f;

    RunSpeed = 1500.0f;

    WeaponSocketName = "WeaponSocket";

    bDied = false;
    bSprint = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

    DefaultFOV = CameraComp->FieldOfView;

    MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
    DefaultSpeed = MoveComp->MaxWalkSpeed;

    HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

    if (Role == ROLE_Authority)
    {
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
}

void ASCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
    if (Value != 0)
    {
        EndSprint();
    }

    AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginSprint()
{
    MoveComp->MaxWalkSpeed = RunSpeed;
    bSprint = true;
}

void ASCharacter::EndSprint()
{
    if (!bSprint)
        return;

    MoveComp->MaxWalkSpeed = DefaultSpeed;
    bSprint = false;
}

void ASCharacter::BeginCrouch()
{
    EndSprint();
    Crouch();
}

void ASCharacter::EndCrouch()
{
    UnCrouch();
}

void ASCharacter::BeginZoom()
{
    EndSprint();
    bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
    bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
    EndSprint();

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

void ASCharacter::OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta,
    const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Health <= 0 && !bDied)
    {
        bDied = true;

        EndSprint();
        EndCrouch();
        EndZoom();
        StopFire();

        GetMovementComponent()->StopMovementImmediately();
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        DetachFromControllerPendingDestroy();

        SetLifeSpan(10.0f);
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

    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASCharacter::BeginSprint);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASCharacter::EndSprint);
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

    if (Role == ROLE_Authority)
        CtrlRotation = GetControlRotation();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied);
	DOREPLIFETIME(ASCharacter, bSprint);
	DOREPLIFETIME(ASCharacter, CtrlRotation);
}