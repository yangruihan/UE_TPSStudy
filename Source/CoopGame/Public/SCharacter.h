// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void MoveForward(float Value);

    void MoveRight(float Value);

    void BeginCrouch();

    void EndCrouch();

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Player")
    float RunSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Player")
    bool bSprint;

    float DefaultSpeed;
    
    void BeginSprint();

    void EndSprint();

    UCharacterMovementComponent* MoveComp;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
    UCameraComponent* CameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
    USpringArmComponent* SpringArmComp;

    /* Zoom */
    bool bWantsToZoom;

    float DefaultFOV;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    float ZoomedFov;

    UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
    float ZoomSpeed;

    void BeginZoom();

    void EndZoom();

protected:

    UPROPERTY(Replicated)
    ASWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    TSubclassOf<ASWeapon> DefaultWeaponClass;

    UPROPERTY(VisibleDefaultsOnly, Category = "Player")
    FName WeaponSocketName;

protected:

    UFUNCTION(BlueprintCallable, Category = "Player")
    void StartFire();

    UFUNCTION(BlueprintCallable, Category = "Player")
    void StopFire();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
    USHealthComponent* HealthComp;

    UFUNCTION()
    void OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta, 
                         const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadonly, Category = "Player")
    bool bDied;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual FVector GetPawnViewLocation() const override;

    virtual void Tick(float DeltaSeconds) override;
};
