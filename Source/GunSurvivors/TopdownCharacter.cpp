

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
    CharacterFlipbook->SetFlipbook(IdleFlipbook);
}

void ATopdownCharacter::Shoot(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("shoot"));
}

