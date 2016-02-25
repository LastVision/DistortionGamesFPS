#include "stdafx.h"
#include "ServerNetworkManager.h"
#include "ServerNetwork.h"
#include <DL_Debug.h>
#include <thread>

#define BUFFERSIZE 512

void ServerNetworkManager::Initiate()
{
	myIsServer = true;
	myNetwork = new ServerNetwork();
	myNetworkID = 0;
	__super::Initiate();
}

void ServerNetworkManager::Create()
{
	if (myInstance == nullptr)
	{
		myInstance = new ServerNetworkManager();
		myInstance->Initiate();
	}
}

void ServerNetworkManager::Destroy()
{
	delete myInstance;
	myInstance = nullptr;
}

ServerNetworkManager* ServerNetworkManager::GetInstance()
{
	if (myInstance != nullptr)
	{
		return myInstance;
	}
	DL_ASSERT("Instance were null, did you forget to create the ServerNetworkManager?");
	return nullptr;
}

void ServerNetworkManager::StartNetwork()
{
	myNetwork->StartServer();
	__super::StartNetwork();

}

ServerNetworkManager::ServerNetworkManager()
{
}


ServerNetworkManager::~ServerNetworkManager()
{
	delete myNetwork;
	myNetwork = nullptr;
}

void ServerNetworkManager::ReceieveThread()
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

void ServerNetworkManager::SendThread()
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
