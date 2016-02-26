#include "stdafx.h"

#include "ClientNetwork.h"
#include "ClientNetworkManager.h"
#include "NetMessageConnectMessage.h"
#include <DL_Debug.h>
#include <thread>

#define BUFFERSIZE 512


ClientNetworkManager* ClientNetworkManager::myInstance = nullptr;

void ClientNetworkManager::ReceieveThread()
{
	char buffer[BUFFERSIZE];
	while (myIsRunning == true)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		ZeroMemory(&buffer, BUFFERSIZE);

		std::vector<Buffer> someBuffers;
		myNetwork->Receieve(someBuffers);

		if (someBuffers.size() == 0)
		{
			int error = WSAGetLastError();
			continue;
		}
		for (Buffer message : someBuffers)
		{
			myReceieveBuffer[myCurrentBuffer ^ 1].Add(message);
		}
	}
}

void ClientNetworkManager::SendThread()
{
	while (myIsRunning == true)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		for (std::vector<char> arr : mySendBuffer[myCurrentSendBuffer])
		{
			myNetwork->Send(arr);
		}

		mySendBuffer[myCurrentSendBuffer].RemoveAll();
		myCurrentSendBuffer ^= 1;
	}
}

void ClientNetworkManager::Initiate()
{
	myIsServer = false;
	myNetwork = new ClientNetwork();
	myNetworkID = 0;
	__super::Initiate();
}

void ClientNetworkManager::Create()
{
	if (myInstance == nullptr)
	{
		myInstance = new ClientNetworkManager();
		myInstance->Initiate();
	}
}

void ClientNetworkManager::Destroy()
{
	delete myInstance;
	myInstance = nullptr;
}

void ClientNetworkManager::StartNetwork()
{
	myNetwork->StartNetwork();
	__super::StartNetwork();
}

void ClientNetworkManager::ConnectToServer(const char* aServerIP)
{
	DL_ASSERT_EXP(myIsServer == false, "Tried to Connect to Server from Server... this doesn't seem right.");
	myIsOnline = myNetwork->ConnectToServer(aServerIP);
	char username[256 + 1];
	DWORD username_len = 256 + 1;
	GetUserNameA(username, &username_len);
	AddMessage(NetMessageConnectMessage(username, -1));
}

ClientNetworkManager* ClientNetworkManager::GetInstance()
{
	if (myInstance != nullptr)
	{
		return myInstance;
	}
	DL_ASSERT("Instance were null, did you forget to create the ClientNetworkManager?");
	return nullptr;
}

ClientNetworkManager::ClientNetworkManager()
{
}

ClientNetworkManager::~ClientNetworkManager()
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
