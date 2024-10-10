

#include "TopdownCharacter.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

ATopdownCharacter::ATopdownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
    
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    SetRootComponent(CapsuleComp);
    
    CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
    CharacterFlipbook->SetupAttachment(CapsuleComp);
    
    GunParent = CreateDefaultSubobject<USceneComponent>(TEXT("GunParent"));
    GunParent->SetupAttachment(CapsuleComp);
    
    GunSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GunSprite"));
    GunSprite->SetupAttachment(GunParent);
    
    BulletSpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnPosition"));
    BulletSpawnPosition->SetupAttachment(GunSprite);
}

void ATopdownCharacter::BeginPlay()
{
	Super::BeginPlay();
    
    // Registering the input mapping context to the player's enhacned input subsystem:
    // Get the PlayerController from the global variable Controller
    APlayerController* PlayerController = Cast<APlayerController>(Controller);
    if (PlayerController)
    {
        // Setting the mouse cursor to be visible
        PlayerController->SetShowMouseCursor(true);
        // Get the player's subsystem
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (Subsystem)
        {
            // Register the mapping context to the subsytem
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
    
    CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ATopdownCharacter::OverlapBegin);
	
}

bool ATopdownCharacter::IsInMapBoundsHorizontal(float XPos)
{
    bool Result = true;
    
    // Result is true if the XPos of the player is within bounds of the HorizontalLimits
    Result = (XPos > HorizontalLimits.X) && (XPos < HorizontalLimits.Y);
    
    return Result;
}

bool ATopdownCharacter::IsInMapBoundsVertical(float ZPos)
{
    bool Result = true;
    
    // Result is true if the ZPos of the player is within bounds of the VerticalLimits
    Result = (ZPos > VerticalLimits.X) && (ZPos < VerticalLimits.Y);
    
    return Result;
}

// Called each frame bu unreal
void ATopdownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    // Moving the player
    if (CanMove)
    {
        // Check if atleast one of the keys are bing pressed by the user
        if (MovementDirection.Length() > 0.0f)
        {
            // Normalise the vector such that we have consistent speeds
            // for different lengths of movement direction vectors. Note when the
            // user pressed two keys at the same time the vector is larger than if only
            // one key is pressed.
            if (MovementDirection.Length() > 1.0f)
            {
                MovementDirection.Normalize();
            }
            
            FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;
            
            FVector CurrentLocation = GetActorLocation();
            // First move horizontally but check if not in map bounds as well
            FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, 0.0f);
            if (!IsInMapBoundsHorizontal(NewLocation.X))
            {
                // Bring back the player to origanl horizontal position
                NewLocation -= FVector(DistanceToMove.X, 0.0f, 0.0f);
            }
            // Now attempt to move vertically
            NewLocation += FVector(0.0f, 0.0f, DistanceToMove.Y);
            if (!IsInMapBoundsVertical(NewLocation.Z))
            {
                // Bring back the player to origanl vertical position
                NewLocation -= FVector(0.0f, 0.0f, DistanceToMove.Y);
            }
            
            // Set the new player location
            SetActorLocation(NewLocation);
        }
    }
    
    // Rotating the gun
    // Get the player controller to get the cursor world coordinates
    APlayerController* PlayerController = Cast<APlayerController>(Controller);
    if (PlayerController)
    {
        FVector MouseWorldLocation, MouseWorldDirection;
        // Arguments are passed by reference
        PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
        
        // Find the angle between the gun parent/player (start location) and cursor (target location)
        FVector CurrentLocation = GetActorLocation();
        FVector Start = FVector(CurrentLocation.X, 0.0f, CurrentLocation.Z);
        FVector Target = FVector(MouseWorldLocation.X, 0.0f, MouseWorldLocation.Z);
        FRotator GunParentRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);
        
        // Set the rotation to the gun parent
        GunParent->SetRelativeRotation(GunParentRotator);
    }

}

// This function is called by Unreal
void ATopdownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind input actions to the functions:
    // Get the enhanced input component
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent)
    {
        // Bind the actions for the move action:
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopdownCharacter::MoveTriggered);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATopdownCharacter::MoveCompleted);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ATopdownCharacter::MoveCompleted);
        
        // Bind the actions for the shoot action:
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATopdownCharacter::Shoot);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATopdownCharacter::Shoot);
    }
    

}

void ATopdownCharacter::MoveTriggered(const FInputActionValue& Value)
{
    FVector2D MoveActionValue = Value.Get<FVector2D>();
    
    if (CanMove)
    {
        MovementDirection = MoveActionValue;
        // Set the flipbook to the run flipbook
        CharacterFlipbook->SetFlipbook(RunFlipbook);
        // Set the direction of the flip book depending on the direction of running:
        FVector FlipbookScale = CharacterFlipbook->GetComponentScale();
        // If going left
        if (MovementDirection.X < 0.0f)
        {
            if (FlipbookScale.X > 0.0f)
            {
                CharacterFlipbook->SetWorldScale3D(FVector(-1.0f, 1.0f, 1.0f));
            }
        }
        // If going right
        else if (MovementDirection.X > 0.0f)
        {
            if (FlipbookScale.X < 0.0f)
            {
                CharacterFlipbook->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
            }
        }
    }
}

void ATopdownCharacter::MoveCompleted(const FInputActionValue& Value)
{
    MovementDirection = FVector2D(0.0f, 0.0f);
    // Reset the player flipbook
    if (IsAlive) CharacterFlipbook->SetFlipbook(IdleFlipbook);
}

void ATopdownCharacter::Shoot(const FInputActionValue& Value)
{
    if (CanShoot)
    {
        CanShoot = false;
        
        // Spawn the bullet actor
        ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletActorToSpawn, BulletSpawnPosition->GetComponentLocation(), FRotator(0.0f, 0.0f, 0.0f));
        
        // Check if the spawning worked
        check(Bullet);
        
        // Get the player controller to get the cursor world coordinates
        APlayerController* PlayerController = Cast<APlayerController>(Controller);
        // Check if PlayerController is valid
        check(PlayerController);
        
        FVector MouseWorldLocation, MouseWorldDirection;
        // Arguments are passed by reference
        PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
        
        // Calculate bullet direction
        FVector CurrentLocation = GetActorLocation();
        FVector2D BulletDirection = FVector2D(MouseWorldLocation.X - CurrentLocation.X, MouseWorldLocation.Z - CurrentLocation.Z);
        // Get rid of the magnitude (we just care about the direction
        BulletDirection.Normalize();
        
        // Launch the bullet
        float BulletSpeed = 300.0f;
        Bullet->Launch(BulletDirection, BulletSpeed);
        
        // Set the timer for gun cool down
        // 1.0f = in rate, false = dont loop, DeleteTime = inital time
        GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATopdownCharacter::OnShootCooldownTimerTimeOut, 1.0f, false, ShootCooldownDurationInSeconds);
        
        // Play the bullet shoot sound
        UGameplayStatics::PlaySound2D(GetWorld(), BulletShootSound);
        
    }
}

void ATopdownCharacter::OnShootCooldownTimerTimeOut()
{
    // Allow the user to shoot again after cool down of gun is finished
    if (IsAlive) CanShoot = true;
}

void ATopdownCharacter::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if the object the player collided with is an enemy
    AEnemy* Enemy = Cast<AEnemy>(OtherActor);
    if (Enemy && Enemy->IsAlive)
    {
        if (IsAlive)
        {
            // Disable the player
            IsAlive = false;
            CanMove = false;
            CanShoot = false;
            
            // Play the die sound
            UGameplayStatics::PlaySound2D(GetWorld(), DieSound);
            
            PlayerDiedDelegate.Broadcast();
        }
    }
}
