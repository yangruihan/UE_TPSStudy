// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;
class UCameraShake;

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    FName MuzzleSocketName;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    FName TracerTargetParamName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    UParticleSystem* MuzzleEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    UParticleSystem* ImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Weapon")
    UParticleSystem* TracerEffect;

    void PlayFireEffect(FVector TracerEndPoint);

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<UCameraShake> CameraShake;

public:	

    UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();
	
};
