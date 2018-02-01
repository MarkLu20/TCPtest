// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Networking.h"
#include "TcpConnection.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TCPCON_API UTcpConnection : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void EndPlay(const EEndPlayReason::Type EEndPlayReason) override;
	
public:
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;

	FIPv4Endpoint RemoteAddressForConnection;

	FTimerHandle TCPSocketListenerTimerHandle;
	FTimerHandle TCPConnectionListenerTimerHandle;

	bool StartTCPReceiver(const FString& YourChosenSokcetName, const FString& TheIP, const int32 ThePort);

	FSocket* CreateTCPconnectionListener(const FString& YourChoseSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize = 2 * 1024 * 1024);

	//Timer Function, could be threads
	void TCPconnectionListener(); //can thread this eventually

	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

	void TCPSokcetListener(); //can thread this eventually; 

	bool FormatIP4ToNumber(const FString& TheIP, uint8(&out)[4]);

	UFUNCTION(BlueprintCallable, Category = "TCPServer")
		bool LaunchTCP();

	UFUNCTION(BlueprintCallable, Category = "TCPServer")
		void TCPSend(FString message);

	UFUNCTION(BlueprintImplementableEvent, Category = "TCPServer")
		void recievedMessage(const FString &message);
	
};
