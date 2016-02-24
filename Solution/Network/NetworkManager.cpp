#include "stdafx.h"

#include "ClientInterface.h"
#include "NetworkManager.h"
#include "NetworkMessageTypes.h"
#include "NetMessageConnectMessage.h"
#include "NetMessageOnJoin.h"
#include "ServerInterface.h"
#include "Utility.h"
#include <DL_Debug.h>
#include <thread>
#include <sstream>

#define BUFFERSIZE 512

#ifdef _DEBUG
#pragma region ThreadNaming
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
void SetThreadName(DWORD dwThreadID, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
#pragma warning(pop)
}

void _SetThreadName(std::thread::id& anID, const char* aThreadName)
{
	std::stringstream ss;
	ss << anID;
	DWORD id;
	ss >> id;
	SetThreadName(id, aThreadName);
}
#pragma endregion
#endif
NetworkManager* NetworkManager::myInstance = nullptr;

void NetworkManager::Activate(bool aIsServer)
{
	myIsRunning = false;
	myCurrentBuffer = 0;
	myReceieveBuffer[0].Init(8);
	myReceieveBuffer[1].Init(8);

	myIsServer = aIsServer;

	myNetwork = new ClientInterface();
	myNetworkID = -1;
	if (myIsServer == true)
	{
		delete myNetwork;
		myNetwork = nullptr;
		myNetwork = new ServerInterface();
		myNetworkID = 0;
	}

}

void NetworkManager::Create(bool aIsServer)
{
	if (myInstance == nullptr)
	{
		myInstance = new NetworkManager();
		myInstance->Activate(aIsServer);
	}
}

void NetworkManager::Destroy()
{
	delete myInstance;
	myInstance = nullptr;
}

void NetworkManager::StartNetwork()
{
	myNetwork->StartNetwork();
	myIsRunning = true;
	myReceieveThread = new std::thread([&]{ReceieveThread(); });

#ifdef _DEBUG
	if (myIsServer == true)
	{
		_SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Server");
	}
	else
	{
		_SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Client");
	}
#endif
	//mySendThread = new std::thread([&]{SendThread(); });
}

void NetworkManager::ConnectToServer(const char* aServerIP)
{
	DL_ASSERT_EXP(myIsServer == false, "Tried to Connect to Server from Server... this doesn't seem right.");
	myNetwork->ConnectToServer(aServerIP);
}

CU::GrowingArray<Buffer>& NetworkManager::GetReceieveBuffer()
{
	return myReceieveBuffer[myCurrentBuffer];
}

NetworkManager* NetworkManager::GetInstance()
{
	if (myInstance != nullptr)
	{
		return myInstance;
	}
	DL_ASSERT("Instance were null, did you forget to create the network manager?");
	return nullptr;
}

void NetworkManager::Swap(bool aShouldSwap)
{
	if (aShouldSwap == true)
	{
		SwapReceieveBuffer();
	}
}

NetworkManager::NetworkManager()
	: myIsServer(false)
{
}

NetworkManager::~NetworkManager()
{
	myIsRunning = false;
	if (myReceieveThread != nullptr)
	{
		myReceieveThread->join();
		delete myReceieveThread;
		myReceieveThread = nullptr;
	}

	if (mySendThread != nullptr)
	{
		mySendThread->join();
		delete mySendThread;
		mySendThread = nullptr;
	}

	delete myNetwork;
	myNetwork = nullptr;
}

void NetworkManager::SendThread()
{

}

void NetworkManager::ReceieveThread()
{
	char buffer[BUFFERSIZE];
	while (myIsRunning == true)
	{
		//Sleep(1);
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		ZeroMemory(&buffer, BUFFERSIZE);
		int receievedLength = myNetwork->Receieve(buffer);

		if (receievedLength < 0)
		{
			int error = WSAGetLastError();
			continue;
		}

		Buffer toInsert;
		toInsert.myData = buffer;
		toInsert.myLength = receievedLength;
		myReceieveBuffer[myCurrentBuffer ^ 1].Add(toInsert);
		if (myIsServer == true)
		{
			Utility::PrintEndl("Server Receieved a message", Utility::eCOLOR::LIGHT_GREEN);
		}



#pragma region Comment
		//eNetMessageType messageType = ReadType(buffer);

		//if (messageType == eNetMessageType::NONE)
		//{
		//	continue;
		//}

		//switch (messageType)
		//{
		//case eNetMessageType::ON_CONNECT:
		//{
		//	NetMessageConnectMessage toUnpack;
		//	toUnpack.UnPackMessage(buffer, receievedLength);
		//	std::string connected = toUnpack.myName + " has connected!";

		//	if (myIsServer == true)
		//	{
		//		myNetwork->CreateConnection(toUnpack.myName);
		//		Utility::PrintEndl(connected, Utility::eCOLOR::LIGHT_GREEN_BACK_BLACK);
		//	}
		//	else
		//	{
		//		myNetworkID = toUnpack.myServerID;
		//		myNetwork->SetIsOnline(true);
		//	}

		//	break;
		//}
		//case eNetMessageType::ON_JOIN:
		//{
		//	//NetMessageOnJoin toUnpack;
		//	//toUnpack.UnPackMessage(buffer, receievedLength);
		//	//toUnpack.myID = buffer[0];
		//	//myBuffer.Add(toUnpack);

		//	break;
		//}
		//}
#pragma endregion

	}
}

eNetMessageType NetworkManager::ReadType(const char* aBuffer)
{
	return static_cast<eNetMessageType>(aBuffer[0]);
}


void NetworkManager::SwapReceieveBuffer()
{
	myReceieveBuffer[myCurrentBuffer].RemoveAll();
	myCurrentBuffer ^= 1;
}