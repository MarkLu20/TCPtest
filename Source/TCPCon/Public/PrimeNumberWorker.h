// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPlayerController.h"

class FPrimeNumberWorker : public FRunnable
{
	//Singleton instance , can access the thread any time via static accessor, if it is active!
	static FPrimeNumberWorker* Runnable;

	// Thread to run the worker FRunnable on
	FRunnableThread* Thread;

	TArray<int32>* PrimeNumbers;
	int32 FindNextPrimeNumber();

	/** The PC */
	ATCPlayerController* ThePC;

	// Stop this thread? Uses Thread Safe Counter 
	FThreadSafeCounter StopTaskCounter;

private:
	int32 PrimesFoundCount;
public:
	int32 TotalPrimesToFind;

	bool IsFinished() const
	{
		return PrimesFoundCount >= TotalPrimesToFind;
	}

	//~~~ Thread Core Functions ~~~
	FPrimeNumberWorker(TArray<int32>& TheArray, const int32 IN_PrimestoFindPerTick, ATCPlayerController* IN_PC);

	
	virtual ~FPrimeNumberWorker();

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();

	//~~~ Starting and Stopping Thread ~~~



	/*
	Start the thread and the worker from static (easy access)!
	This code ensures only 1 Prime Number thread will be able to run at a time.
	This function returns a handle to the newly started instance.
	*/
	static FPrimeNumberWorker* JoyInit(TArray<int32>& TheArray, const int32 IN_TotalPrimesToFind, ATCPlayerController* IN_PC);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();

	static bool IsThreadFinished();
};
