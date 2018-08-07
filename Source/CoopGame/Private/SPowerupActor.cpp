// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
    TickInterval = 0.0f;
    TickTimes = 0;
    bActive = false;
    CurrentTickTimes = 0;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
    Super::BeginPlay();
}

void ASPowerupActor::PowerupTick()
{
    OnPowerupTicked();

    CurrentTickTimes++;
    if (CurrentTickTimes >= TickTimes)
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PowerupTick);

        OnPowerupExpired();
    }
}

void ASPowerupActor::Active()
{
    OnPowerupActivated();

    if (TickTimes > 0)
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::PowerupTick, TickInterval, true, TickInterval);
    }
    else
    {
        OnPowerupTicked();
        OnPowerupExpired();
    }
}
