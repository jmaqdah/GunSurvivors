
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"

#include "Engine/TimerHandle.h"

#include "TopdownCharacter.h"

#include "Enemy.generated.h"

UCLASS()
class GUNSURVIVORS_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UCapsuleComponent* CapsuleComp;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UPaperFlipbookComponent* EnemyFlipbook;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UPaperFlipbook* DeadFlipbookAsset;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ATopdownCharacter* Player;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool IsAlive = true;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool CanFollow = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovementSpeed = 50.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StopDistance = 20.0f;
    
    FTimerHandle DestroyTimer;
    
	AEnemy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
    
    void Die();
    
    void OnDestroyTimerTimeOut();

};
