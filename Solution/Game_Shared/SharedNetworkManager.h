#pragma once
#include <vector>
#include <GrowingArray.h>
#include <StaticArray.h>
#include <NetworkMessageTypes.h>
#include <NetMessageConnectMessage.h>
namespace std
{
	class thread;
}
class Message;
class SharedNetworkManager
{
public:

	virtual void Initiate();
	virtual void StartNetwork();
	void Update(float aDelta);

	template<typename T>
	void AddMessage(T& aMessage);
	
	eNetMessageType ReadType(const char* aBuffer);
	unsigned short GetResponsTime() const;

protected:
	void AddNetworkMessage(std::vector<char> aBuffer);


	SharedNetworkManager();
	virtual ~SharedNetworkManager();
	
	virtual void SendThread() = 0;
	virtual void ReceieveThread() = 0;

	void SwapBuffers();

	void HandleMessage();
	virtual void HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress);






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

	
	float myPingTime;
	unsigned short myResponsTime;
	float myMS;

};

template<typename T>
inline void SharedNetworkManager::AddMessage(T& aMessage)
{
	aMessage.PackMessage();
	AddNetworkMessage(aMessage.myStream);
}

