#pragma once
#include <StaticArray.h>
#include <GrowingArray.h>
#include "BaseNetwork.h"
#include "NetMessage.h"
enum class eNetMessageType;
namespace std
{
	class thread;
}

class NetworkManager
{
public:

	void Activate(bool aIsServer);
	static void Create(bool aIsServer);
	static void Destroy();
	static NetworkManager* GetInstance();

	void SendMessage();

	void StartNetwork();
	void ConnectToServer();
	
	CU::GrowingArray<NetMessage>& GetBuffer();

	eNetMessageType ReadType(const char* aBuffer);
private:
	NetworkManager();
	~NetworkManager();

	void SendThread();
	void ReceieveThread();

	//CU::StaticArray<CU::GrowingArray<NetMessage>, 2> myReceieveBuffer;
	//CU::StaticArray<CU::GrowingArray<NetMessage>, 2> mySendBUffer;

	CU::GrowingArray<NetMessage> myBuffer;


	std::thread* myReceieveThread;
	std::thread* mySendThread;

	BaseNetwork* myNetwork;


	static NetworkManager* myInstance;
	bool myIsServer;
	volatile bool myIsRunning;
	short myNetworkID;

};

