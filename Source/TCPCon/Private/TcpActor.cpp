// Fill out your copyright notice in the Description page of Project Settings.

#include "TcpActor.h"
#include <string>

// Sets default values
ATcpActor::ATcpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATcpActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATcpActor::EndPlay(const EEndPlayReason::Type EEndPlayReason)
{
	Super::EndPlay(EEndPlayReason);

	if (ListenSocket != NULL)
	{
		ListenSocket->Close();
	}
}

// Called every frame
void ATcpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TCPSokcetListener();
}


//Start UDP Receiver
void ATcpActor::StartTCPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, bool& success) // 接收器初始化  接收信息前
{

	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	int uiIP = (127 << 24) | 0 | 0 | 1;
	targetAddr->SetIp(uiIP);
	targetAddr->SetPort(ThePort);

	FIPv4Endpoint ipadd(targetAddr);

	ListenSocket = FTcpSocketBuilder(TEXT("FTcpMessageTransport.RemoteConnection"));
	if (ListenSocket && ListenSocket->Connect(ipadd.ToInternetAddr().Get())){}
}

void ATcpActor::TCPSokcetListener()
{
	//~~~~~~~~~~~~~
	if (!ListenSocket) return;
	//~~~~~~~~~~~~~

	//Binary Array!
	TArray<uint8> ReceivedData;

	uint32 Size;
	while (ListenSocket->HasPendingData(Size))
	{
		ReceivedData.Init(FMath::Min(Size, 65507u), Size);

		int32 Read = 0;
		ListenSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (ReceivedData.Num() <= 0)
	{
		return;
	}

	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);

	//callback to blueprint
	recievedMessage(ReceivedUE4String);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
}

FString ATcpActor::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	//Create a string from a byte array!
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());

	return FString(cstr.c_str());
}

void ATcpActor::TCPSend(FString message)
{
	//message = message + LINE_TERMINATOR; //For Matlab we need a defined line break (fscanf function) "\n" ist not working, therefore use the LINE_TERMINATOR macro form UE

	TCHAR *SerializedChar = message.GetCharArray().GetData();
	int32 Size = FCString::Strlen(SerializedChar);
	int32 Sent = 0;
	uint8* ResultChars = (uint8*)TCHAR_TO_UTF8(SerializedChar);

	if (!ListenSocket->Send(ResultChars, Size, Sent)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error sending message")));
	}
}

void ATcpActor::TCPSokcetListenerAysn()
{
	(new FAutoDeleteAsyncTask<TCPReceiveAsynTask>(this))->StartBackgroundTask();
}