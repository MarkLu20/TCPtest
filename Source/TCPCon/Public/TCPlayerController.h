// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TCPCON_API ATCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Test")
		void Test();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
		TArray<int32> PrimeNumbers;

};
