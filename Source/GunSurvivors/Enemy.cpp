
#include "Enemy.h"

#include "Kismet/GameplayStatics.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
    
    // Create the capsule component and set it as the root
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    SetRootComponent(CapsuleComp);
    
    // Create the flibook component and attach it to the root component
    EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
    EnemyFlipbook->SetupAttachment(RootComponent);

}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();
    
    if (!Player)
    {
        // Look at all the actors in the game and return a pointer to a ATopdownCharacter that has been found
        AActor* PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopdownCharacter::StaticClass());
        if (PlayerActor)
        {
            // If a ATopdownCharacter has been found, set it to Player
            Player = Cast<ATopdownCharacter>(PlayerActor);
            // If Player is found then the enemy can follow the player
            CanFollow = true;
        }
    }
	
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    // Follow the player
    if (IsAlive && CanFollow && Player)
    {
        // Move towards player
        FVector CurrentLocation = GetActorLocation();
        FVector PlayerLocation = Player->GetActorLocation();
        
        // Find the directon of the Player from the Enemy
        FVector DirectionToPlayer = PlayerLocation - CurrentLocation;
        float DistanceToPlayer = DirectionToPlayer.Length();
        
        // Check if the Enemy is fare enough from the Player
        if (DistanceToPlayer >= StopDistance)
        {
            // Get the direction
            DirectionToPlayer.Normalize();
            // Find the new location
            FVector NewLocation = CurrentLocation + (DirectionToPlayer * MovementSpeed * DeltaTime);
            // Set the new location
            SetActorLocation(NewLocation);
            
        }
        
        // Face the player
        // Get the new location of the enemy
        CurrentLocation = GetActorLocation();
        float FlipbookXScale = EnemyFlipbook->GetComponentScale().X;
        
        if ((PlayerLocation.X - CurrentLocation.X) >= 0.0f) // Player is on the right side of the enemy
        {
            if (FlipbookXScale < 0.0f)
            {
                EnemyFlipbook->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
            }
        }
        else // Player is on the left side of the enemy
        {
            if (FlipbookXScale > 0.0f)
            {
                EnemyFlipbook->SetWorldScale3D(FVector(-1.0f, 1.0f, 1.0f));
            }
        }
        
    }

}


void AEnemy::Die()
{
    if (!IsAlive) return;
    
    // Stop enemy from interacting with the bullet
    IsAlive = false;
    
    // Stop the enemy from following the player
    CanFollow = false;
    
    // Set the dead flipbook
    EnemyFlipbook->SetFlipbook(DeadFlipbookAsset);
    
    // Decrease the sort priority such that the alive enemies are seen on top of the dead ones
    EnemyFlipbook->SetTranslucentSortPriority(-5);
    
    // Set the timer to remove the dead enemy
    float DestroyTime = 10.0f;
    GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy::OnDestroyTimerTimeOut, 1.0f, false, DestroyTime);
}

void AEnemy::OnDestroyTimerTimeOut()
{
    Destroy();
}

