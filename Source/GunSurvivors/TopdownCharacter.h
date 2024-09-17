#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"

#include "TopdownCharacter.generated.h"

UCLASS()
class GUNSURVIVORS_API ATopdownCharacter : public APawn
{
	GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UCapsuleComponent* CapsuleComp;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UPaperFlipbookComponent* CharacterFlipbook;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputMappingContext* InputMappingContext;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputAction* MoveAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputAction* ShootAction;
    
	ATopdownCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    void MoveTriggered(const FInputActionValue& Value);
    
    void MoveCompleted(const FInputActionValue& Value);
    
    void Shoot(const FInputActionValue& Value);
    

};
