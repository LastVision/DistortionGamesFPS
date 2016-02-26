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

	myClients.Init(16);
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
	myIsOnline = true;
}

ServerNetworkManager::ServerNetworkManager()
{
}


ServerNetworkManager::~ServerNetworkManager()
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
			//Utility::PrintEndl("Server receieved a message.", LIGHT_GREEN_TEXT);
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
			for (Connection& connection : myClients)
			{
				myNetwork->Send(arr, connection.myAddress);
			}
		}
		mySendBuffer[myCurrentSendBuffer].RemoveAll();
		myCurrentSendBuffer ^= 1;
	}
}

void ServerNetworkManager::CreateConnection(const std::string& aName, const sockaddr_in& aSender)
{
	/*if (myNames.find(aName) == myNames.end())
	{*/
	for (Connection& connection : myClients)
	{
		if (connection.myAddress.sin_addr.S_un.S_addr == aSender.sin_addr.S_un.S_addr) //._.
		{
			Utility::PrintEndl("User already connected!", (DARK_RED_BACK | WHITE_TEXT));
			return;
		}
	}
	myIDCount++;
	Connection newConnection;
	newConnection.myAddress = aSender;
	newConnection.myID = myIDCount;
	newConnection.myName = aName;
	newConnection.myPingCount = 0;
	newConnection.myIsConnected = true;
	myClients.Add(newConnection);
	myNames[aName] = 1;

	std::string conn(aName + " connected to the server!");
	Utility::PrintEndl(conn, LIGHT_GREEN_TEXT);

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