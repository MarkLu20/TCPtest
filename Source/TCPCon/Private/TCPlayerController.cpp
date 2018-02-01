// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPlayerController.h"
#include "PrimeNumberWorker.h"



void ATCPlayerController::Test()
{
	FPrimeNumberWorker::JoyInit(PrimeNumbers, 50000, this);

	//FString result = FString::FromInt(PrimeNumbers.Last());

	//sreturn result;
}
