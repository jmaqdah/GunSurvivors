

#include "Bullet.h"

#include "Enemy.h"

// Sets default values
ABullet::ABullet()
{

	PrimaryActorTick.bCanEverTick = true;
    
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SetRootComponent(SphereComp);
    
    BulletSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BulletSprite"));
    BulletSprite->SetupAttachment(RootComponent);
    
    MovementDirection = FVector2D(1.0f, 0.0f);

}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
    
    // Bind the overlap event of the sphere component to the OverlapBegin function
    SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OverlapBegin);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    // Bullet movement
    if (IsLaunched)
    {
        FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;
        
        FVector CurrentLocation = GetActorLocation();
        
        FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, DistanceToMove.Y);
        
        SetActorLocation(NewLocation);
    }

}

void ABullet::Launch(FVector2D Direction, float Speed)
{
    // If the bullet is already launched then return
    if (IsLaunched) return;
    
    IsLaunched = true;
    
    MovementDirection = Direction;
    MovementSpeed = Speed;
    
    float DeleteTime = 10.0f;
    // 1.0f = in rate, false = dont loop, DeleteTime = inital time
    GetWorldTimerManager().SetTimer(DeleteTimer, this, &ABullet::OnDeleteTimerTimeout, 1.0f, false, DeleteTime);
}

void ABullet::OnDeleteTimerTimeout()
{
    // Destroy the Bullet Actor
    Destroy();
}

void ABullet::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if the actor we collided with is an enemy actor
    AEnemy* Enemy = Cast<AEnemy>(OtherActor);
    if (Enemy && Enemy->IsAlive)
    {
        // Disable bullet
        DisableBullet();
        // Kill enemy
        Enemy->Die();
    }
}

void ABullet::DisableBullet()
{
    if (IsDisabled) return;
    
    IsDisabled = true;
    
    // Make the bullet no longer collide with anything else
    SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Destroy the sprite (i.e., make the bullet invisible)
    BulletSprite->DestroyComponent();
    
}
