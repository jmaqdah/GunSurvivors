

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
	
}

void ATopdownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATopdownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

