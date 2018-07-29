// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"


// Sets default values
ATrackerBot::ATrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCanEverAffectNavigation(false);
    RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ATrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ATrackerBot::GetNextPathPoint()
{
    auto PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (PlayerPawn)
    {
        auto NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
        if (NavPath && NavPath->PathPoints.Num() > 1)
        {
            return NavPath->PathPoints[1];
        }
    }
    
    return GetActorLocation();
}

// Called every frame
void ATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
