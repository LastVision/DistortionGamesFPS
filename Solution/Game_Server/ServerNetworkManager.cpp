#include "stdafx.h"
#include "ServerNetworkManager.h"
#include "ServerNetwork.h"
#include <DL_Debug.h>
#include <thread>
#include <Utility.h>

#define BUFFERSIZE 512

ServerNetworkManager* ServerNetworkManager::myInstance = nullptr;

void ServerNetworkManager::Initiate()
{
	myIsServer = true;
	myNetwork = new ServerNetwork();
	myNetworkID = 0;
	myIDCount = 0;
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

void ServerNetworkManager::HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress)
{
	CreateConnection(aMessage.myName, aSenderAddress);
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
			Utility::PrintEndl("Server receieved a message.",Utility::eCOLOR::LIGHT_GREEN);
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

void ServerNetworkManager::CreateConnection(const std::string& aName, const sockaddr_in& aSender)
{
	/*if (myNames.find(aName) == myNames.end())
	{*/
	myIDCount++;
	Connection newConnection;
	newConnection.myAddress = aSender;
	newConnection.myID = myIDCount;
	newConnection.myName = aName;
	newConnection.myPingCount = 0;
	newConnection.myIsConnected = true;
	myClients.Add(newConnection);
	myNames[aName] = 1;
	

	//NetMessageConnectMessage toSend;
	//toSend.Init(aName, myIDCount);
	//for (Connection c : myClients)
	//{
	//	toSend.myClientsOnServer.Add(c.myID);
	//}
	//toSend.PackMessage();
	//Send(toSend.myStream, newConnection.myAdress);

	//NetMessageOnJoin onJoin;
	//Send(onJoin);
	/*}
	else
	{
	Utility::PrintEndl("User is already on server!", Utility::eCOLOR::RED_BACK_BLACK);
	}*/
}