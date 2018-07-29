// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Public/SHealthComponent.h"

// Sets default values
ATrackerBot::ATrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCanEverAffectNavigation(false);
    MeshComp->SetSimulatePhysics(true);
    RootComponent = MeshComp;
    
    HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
    HealthComp->DefaultHealth = 100.0f;
    
    bUseVelocityChagne = true;
    ReachRequiredDistance = 100.0f;
    MovementForce = 1000.0f;
}

// Called when the game starts or when spawned
void ATrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
    HealthComp->OnHealthChanged.AddDynamic(this, &ATrackerBot::OnHealthChanged);
    
    NextPathPoint = GetNextPathPoint();
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

    auto Vector = NextPathPoint - GetActorLocation();
    auto Distance = Vector.Size();
    if (Distance <= ReachRequiredDistance)
    {
        NextPathPoint = GetNextPathPoint();
    }
    else
    {
        auto ForceDirection = Vector;
        ForceDirection.Normalize();
        ForceDirection *= MovementForce;
        MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChagne);
    }
    
    DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
}

void ATrackerBot::OnHealthChanged(USHealthComponent* HealthCom, float Health, float HealthDelta,
                                  const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (%s)"), *FString::SanitizeFloat(Health), *GetName());
}
