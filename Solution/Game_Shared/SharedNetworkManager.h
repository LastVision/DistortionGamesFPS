#pragma once
#include <vector>
#include <GrowingArray.h>
#include <StaticArray.h>
#include <NetworkMessageTypes.h>
#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>



namespace std
{
	class thread;
}

class SharedNetworkManager
{
public:

	virtual void Initiate();
	virtual void StartNetwork();
	virtual void Update(float aDelta);

	template<typename T>
	void AddMessage(T& aMessage);
	
	eNetMessageType ReadType(const char* aBuffer);
	eNetMessageType ReadType(const std::vector<char>& aBuffer);
	unsigned short GetResponsTime() const;
	double GetDataSent() const;

	void SwapBuffers();

	void ReceieveIsDone();
	void MainIsDone();

	void WaitForMain();
	void WaitForReceieve();


protected:
	void AddNetworkMessage(std::vector<char> aBuffer);


	SharedNetworkManager();
	virtual ~SharedNetworkManager();
	
	virtual void SendThread() = 0;
	virtual void ReceieveThread() = 0;

	template<typename T> 
	void UnpackAndHandle(T& aMessage, Buffer& aBuffer);
	void HandleMessage();

	virtual void HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress);

	std::thread* myReceieveThread;
	std::thread* mySendThread;

	CU::StaticArray<CU::GrowingArray<Buffer>, 2> myReceieveBuffer;
	CU::StaticArray<CU::GrowingArray<std::vector<char>>, 2> mySendBuffer;

	bool myIsServer;
	bool myIsOnline;

	unsigned short myCurrentBuffer;
	unsigned short myCurrentSendBuffer;

	volatile bool myIsRunning;

	volatile bool myReceieveIsDone;
	volatile bool myMainIsDone;



	unsigned short myNetworkID;

	
	float myPingTime;
	float myResponsTime;
	float myMS;

	double myDataSent;
	double myDataToPrint;

};

template<typename T>
inline void SharedNetworkManager::AddMessage(T& aMessage)
{
	if (myIsServer == false)
	{
		aMessage.mySenderID = myNetworkID;
	}
	aMessage.PackMessage();
	myDataSent += aMessage.myStream.size() * sizeof(char);
	AddNetworkMessage(aMessage.myStream);
}

template<typename T>
void SharedNetworkManager::UnpackAndHandle(T& aMessage, Buffer& aBuffer)
{
	aMessage.UnPackMessage(aBuffer.myData, aBuffer.myLength);
	HandleMessage(aMessage, aBuffer.mySenderAddress);
}

