// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "TcpActor.generated.h"

UCLASS()
class TCPCON_API ATcpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATcpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EEndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	FSocket* ListenSocket;

	UFUNCTION(BlueprintCallable, Category = "UDP")
		void StartTCPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, bool& success);

	UFUNCTION(BlueprintImplementableEvent, Category = "TCPServer")
		void recievedMessage(const FString &message);


	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

	UFUNCTION(BlueprintCallable, Category = "TCPServer")
		void TCPSokcetListener();

	UFUNCTION(BlueprintCallable, Category = "TCPServer")
		void TCPSend(FString message);

	UFUNCTION(BlueprintCallable, Category = "TCPServer")
		void TCPSokcetListenerAysn();
};


class TCPReceiveAsynTask : public FNonAbandonableTask
{

	ATcpActor* tcp;

public:
	TCPReceiveAsynTask(ATcpActor* TCP_IN)
	{
		tcp = TCP_IN;
	}

	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(PrimeCalculationAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork()
	{
		tcp->TCPSokcetListener();

		GLog->Log("--------------------------------------------------------------------");
		GLog->Log("End of TCPSokcetListener on background thread");
		GLog->Log("--------------------------------------------------------------------");
	}


};