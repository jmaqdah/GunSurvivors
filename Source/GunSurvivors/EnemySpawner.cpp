
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
    
    // Look at all the actors in the game and return a pointer to a ATopdownCharacter that has been found
    AActor* PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopdownCharacter::StaticClass());
    if (PlayerActor)
    {
        // If a ATopdownCharacter has been found, set it to Player
        Player = Cast<ATopdownCharacter>(PlayerActor);
    }
    
    StartSpawning();
	
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEnemySpawner::StartSpawning()
{
    // Every SpawnTime [s] we want to spawn an enemy
    GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::OnSpawnTimerTimeout, SpawnTime, true, SpawnTime);
}

void AEnemySpawner::StopSpawning()
{
    GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AEnemySpawner::OnSpawnTimerTimeout()
{
    SpawnEnemy();
}

void AEnemySpawner::SpawnEnemy()
{
    // Spawn the enemy:
    // Get a random position in a unit circle
    FVector2D RandomPosition = FVector2D(FMath::VRand());
    // Normalise the position to get the position at the edge of the circle
    RandomPosition.Normalize();
    // Get the final position relative to the EnemySpawner
    RandomPosition *= SpawnDistance;
    FVector EnemyLocation = GetActorLocation() + FVector(RandomPosition.X, 0.0f, RandomPosition.Y);
    // Spawn an enemy at EnemyLocation
    AEnemy* Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyActorToSpawn, EnemyLocation, FRotator::ZeroRotator);
    // Set up the enemy
    SetupEnemy(Enemy);
    
    
    // Increase the difficulty:
    TotalEnemyCount++;
    // If we are at a count that is divisible by DifficultySpikeInterval then attempt to increase diffculty
    if ((TotalEnemyCount % DifficultySpikeInterval) == 0)
    {
        // Dont make it harder if the limit is met
        if (SpawnTime > SpawnTimeMinimumLimit)
        {
            // Spawning more frequent
            SpawnTime -= DecreaseSpawnTimerByEveryInterval;
            
            if (SpawnTime < SpawnTimeMinimumLimit)
                SpawnTime = SpawnTimeMinimumLimit;
            
            // We have to stop and start the timer to update the interval time
            StopSpawning();
            StartSpawning();
        }
    }
}

void AEnemySpawner::SetupEnemy(AEnemy* Enemy)
{
    if (Enemy)
    {
        // Assign the player to the enemy and allow it to move
        Enemy->Player = Player;
        Enemy->CanFollow = true;
    }
}

