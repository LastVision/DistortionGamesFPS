#pragma once
#include <vector>
#include <NetworkMessageTypes.h>

namespace std
{
	class thread;
}

class SharedNetworkManager
{
public:

	virtual void Initiate();
	virtual void StartNetwork();
	template<typename T>
	void AddMessage(T& aMessage);
	eNetMessageType ReadType(const char* aBuffer);

protected:
	void AddMessage(std::vector<char> aBuffer);

	SharedNetworkManager();
	virtual ~SharedNetworkManager();
	
	virtual void SendThread() = 0;
	virtual void ReceieveThread() = 0;

	void SwapBuffers();


	std::thread* myReceieveThread;
	std::thread* mySendThread;

	CU::StaticArray<CU::GrowingArray<Buffer>, 2> myReceieveBuffer;
	CU::StaticArray<CU::GrowingArray<std::vector<char>>, 2> mySendBuffer;

	bool myIsServer;
	bool myIsOnline;

	unsigned short myCurrentBuffer;
	unsigned short myCurrentSendBuffer;

	volatile bool myIsRunning;

	unsigned short myNetworkID;

};

template<typename T>
void SharedNetworkManager::AddMessage(T& aMessage)
{
	aMessage.PackMessage();
	AddMessage(aMessage.myStream);
}

