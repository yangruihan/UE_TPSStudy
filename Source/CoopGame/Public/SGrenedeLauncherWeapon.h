// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SGrenedeLauncherWeapon.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGrenedeLauncherWeapon : public ASWeapon
{
	GENERATED_BODY()
	
protected:

    UPROPERTY(EditDefaultsOnly, Category = "GrenedeLauncherWeapon")
    TSubclassOf<AActor> ProjectileClass;

    virtual void Fire() override;
	
	
};
