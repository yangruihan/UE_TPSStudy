// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Powerup")
    float TickInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Powerup")
    int32 TickTimes;
    
    UPROPERTY(BlueprintReadonly, Category = "Powerup")
    bool bActive;

    int32 CurrentTickTimes;
    
    FTimerHandle TimerHandle_PowerupTick;

    void PowerupTick();

public:	

    void Active();

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
    void OnPowerupActivated();

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
    void OnPowerupTicked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
    void OnPowerupExpired();
};
