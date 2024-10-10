// Fill out your copyright notice in the Description page of Project Settings.


#include "GunSurvivorsGameMode.h"

#include "Kismet/GameplayStatics.h"

AGunSurvivorsGameMode::AGunSurvivorsGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    
    SetScore(0);
}

void AGunSurvivorsGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    SetScore(0);
}

void AGunSurvivorsGameMode::SetScore(int NewScore)
{
    if (NewScore >= 0)
    {
        Score = NewScore;
        
        ScoreChangedDelegate.Broadcast(Score);
    }
}

void AGunSurvivorsGameMode::AddScore(int AmountToAdd)
{
    int NewScore = Score + AmountToAdd;
    SetScore(NewScore);
}

void AGunSurvivorsGameMode::RestartGame()
{
    GetWorldTimerManager().SetTimer(RestartGameTimer, this, &AGunSurvivorsGameMode::OnRestartGameTimerTimeout, 1, false, TimeBeforeRestart);
}

void AGunSurvivorsGameMode::OnRestartGameTimerTimeout()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("MainLevel"));
}

