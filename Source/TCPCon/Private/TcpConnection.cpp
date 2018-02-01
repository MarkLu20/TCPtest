// Fill out your copyright notice in the Description page of Project Settings.

#include "TcpConnection.h"
//#include "Vection.h"
#include <string>




void UTcpConnection::EndPlay(const EEndPlayReason::Type EEndPlayReason)
{
	Super::EndPlay(EEndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(TCPConnectionListenerTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(TCPSocketListenerTimerHandle);

	if (ConnectionSocket != NULL)
	{
		ConnectionSocket->Close();
	}
	if (ListenerSocket != NULL)
	{
		ListenerSocket->Close();
	}
}

bool UTcpConnection::LaunchTCP()
{
	if (!StartTCPReceiver("MySocketListener", "127.0.0.1", 62))
	{
		return false;
	}

	return true;
}

bool UTcpConnection::StartTCPReceiver(const FString& YourChosenSokcetName, const FString& TheIP, const int32 ThePort)
{
	ListenerSocket = CreateTCPconnectionListener(YourChosenSokcetName, TheIP, ThePort);

	if (!ListenerSocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *TheIP, ThePort));
		return false;
	}

	//Start the Listener! //thread this eventually

	UWorld* world = GetWorld();

	world->GetTimerManager().SetTimer(TCPConnectionListenerTimerHandle, this, &UTcpConnection::TCPconnectionListener, 1.0f, true);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket created")));
	return true;
}

bool UTcpConnection::FormatIP4ToNumber(const FString& TheIP, uint8(&out)[4])
{
	//IP Formatting
	TheIP.Replace(TEXT(" "), TEXT(""));

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//						   IP 4 Parts

	//String Parts
	TArray<FString> Parts;
	TheIP.ParseIntoArray(Parts, TEXT("."), true);
	if (Parts.Num() != 4)
		return false;

	//String to Number Parts
	for (int32 i = 0; i < 4; ++i)
	{
		out[i] = FCString::Atoi(*Parts[i]);
	}

	return true;
}

FSocket* UTcpConnection::CreateTCPconnectionListener(const FString& YourChoseSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize /* = 2 * 1024 * 1024 */)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(TheIP, IP4Nums))
	{
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), ThePort);
	FSocket* ListenSocket = FTcpSocketBuilder(*YourChoseSocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	//Done!
	return ListenSocket;
}

void UTcpConnection::TCPconnectionListener()
{
	//~~~~~~~~~~~~~
	if (!ListenerSocket) return;
	//~~~~~~~~~~~~~
	//Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	// handle incoming connections
	ListenerSocket->HasPendingConnection(Pending);

	if (Pending)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//Already have a Connection? destroy previous
		if (ConnectionSocket)
		{
			ConnectionSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//New Connection receive!
		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("RamaTCP Received Socket Connection"));


		if (ConnectionSocket != NULL)
		{
			//Global cache of current Remote Address
			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);

			//can thread this too
			UWorld* World = GetWorld();

			World->GetTimerManager().SetTimer(TCPSocketListenerTimerHandle, this, &UTcpConnection::TCPSokcetListener, 0.1f, true);
		}
	}
}

FString UTcpConnection::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	//Create a string from a byte array!
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());

	return FString(cstr.c_str());
}

void UTcpConnection::TCPSend(FString message)
{
	//message = message + LINE_TERMINATOR; //For Matlab we need a defined line break (fscanf function) "\n" ist not working, therefore use the LINE_TERMINATOR macro form UE

	TCHAR *SerializedChar = message.GetCharArray().GetData();
	int32 Size = FCString::Strlen(SerializedChar);
	int32 Sent = 0;
	uint8* ResultChars = (uint8*)TCHAR_TO_UTF8(SerializedChar);

	if (!ConnectionSocket->Send(ResultChars, Size,Sent)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error sending message")));
	}
}

void UTcpConnection::TCPSokcetListener()
{
	//~~~~~~~~~~~~~
	if (!ConnectionSocket) return;
	//~~~~~~~~~~~~~


	//Binary Array!
	TArray<uint8> ReceivedData;

	uint32 Size;
	while (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(FMath::Min(Size, 65507u), Size);

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (ReceivedData.Num() <= 0)
	{
		return;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//						Rama's String From Binary Array
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	recievedMessage(ReceivedUE4String);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
}