#pragma once
#include <vector>
#include <GrowingArray.h>
#include <StaticArray.h>
#include <NetworkMessageTypes.h>
#include <NetMessageConnectMessage.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>

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
	eNetMessageType ReadType(const std::vector<char>& aBuffer);
	float GetResponsTime() const;

	void SwapBuffers();
protected:
	void AddNetworkMessage(std::vector<char> aBuffer);


	SharedNetworkManager();
	virtual ~SharedNetworkManager();
	
	virtual void SendThread() = 0;
	virtual void ReceieveThread() = 0;


	void HandleMessage();

	virtual void HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress);

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
	float myResponsTime;
	float myMS;

};

template<typename T>
inline void SharedNetworkManager::AddMessage(T& aMessage)
{
	aMessage.PackMessage();
	AddNetworkMessage(aMessage.myStream);
}

