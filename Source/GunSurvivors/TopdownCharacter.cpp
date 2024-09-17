

#include "TopdownCharacter.h"

ATopdownCharacter::ATopdownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
    
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    SetRootComponent(CapsuleComp);
    
    CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
    CharacterFlipbook->SetupAttachment(CapsuleComp);
}

void ATopdownCharacter::BeginPlay()
{
	Super::BeginPlay();
    
    // Registering the input mapping context to the player's enhacned input subsystem:
    // Get the PlayerController from the global variable Controller
    APlayerController* PlayerController = Cast<APlayerController>(Controller);
    if (PlayerController)
    {
        // Get the player's subsystem
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (Subsystem)
        {
            // Register the mapping context to the subsytem
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
	
}

// Called each frame bu unreal
void ATopdownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
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
            FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, DistanceToMove.Y);
            
            SetActorLocation(NewLocation);
        }
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
    }
}

void ATopdownCharacter::MoveCompleted(const FInputActionValue& Value)
{
    MovementDirection = FVector2D(0.0f, 0.0f);
}

void ATopdownCharacter::Shoot(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("shoot"));
}

