#pragma once
#include <StaticArray.h>
#include <GrowingArray.h>
#include "BaseNetwork.h"
#include "NetMessage.h"
enum class eNetMessageType;
struct Buffer
{
	char* myData;
	int myLength;
};


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
	void Swap(bool aShouldSwap);
	void SendMessage();

	void StartNetwork();
	void ConnectToServer(const char* aServerIP = "127.0.0.1");
	
	CU::GrowingArray<Buffer>& GetReceieveBuffer();

	eNetMessageType ReadType(const char* aBuffer);

private:
	NetworkManager();
	~NetworkManager();

	void SendThread();
	void ReceieveThread();

	void SwapReceieveBuffer();
	
	CU::StaticArray<CU::GrowingArray<Buffer>, 2> myReceieveBuffer;
	//CU::StaticArray<CU::GrowingArray<NetMessage>, 2> mySendBUffer;

	//CU::GrowingArray<NetMessage> myBuffer;


	std::thread* myReceieveThread;
	std::thread* mySendThread;

	BaseNetwork* myNetwork;

	unsigned short myCurrentBuffer;
	int myWarningCount;
	static NetworkManager* myInstance;
	bool myIsServer;
	volatile bool myIsRunning;
	short myNetworkID;

};

