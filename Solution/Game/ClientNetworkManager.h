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

class ClientNetworkManager
{
public:

	void Activate(bool aIsServer);
	static void Create(bool aIsServer);
	static void Destroy();
	static ClientNetworkManager* GetInstance();

	void AddMessage(std::vector<char> aBuffer);



	void Swap(bool aShouldSwap);
	void SendMessage();

	void StartNetwork();
	void ConnectToServer(const char* aServerIP = "127.0.0.1");
	
	CU::GrowingArray<Buffer>& GetReceieveBuffer();

	eNetMessageType ReadType(const char* aBuffer);

	BaseNetwork* GetNetworkHandle();
	void SetNetworkID(unsigned short anID);
	unsigned short GetNetworkID();

private:
	ClientNetworkManager();
	~ClientNetworkManager();
	static ClientNetworkManager* myInstance;

	void SendThread();
	void ReceieveThread();

	void SwapReceieveBuffer();
	
	CU::StaticArray<CU::GrowingArray<Buffer>, 2> myReceieveBuffer;
	CU::StaticArray<CU::GrowingArray<std::vector<char>>, 2> mySendBuffer;

	std::thread* myReceieveThread;
	std::thread* mySendThread;

	BaseNetwork* myNetwork;

	unsigned short myCurrentBuffer;
	unsigned short myCurrentSendBuffer;

	int myWarningCount;

	volatile bool myReceieveIsDone;
	volatile bool myIsRunning;

	bool myIsServer;
	unsigned short myNetworkID;

};

