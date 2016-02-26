#include "stdafx.h"

#include "ClientNetwork.h"
#include "ClientNetworkManager.h"
#include "NetMessageConnectMessage.h"
#include <DL_Debug.h>
#include <thread>
#include "NetworkMessageTypes.h"
#include <PostNetPositionMessage.h>
#include <PostMaster.h>
#define BUFFERSIZE 512


ClientNetworkManager* ClientNetworkManager::myInstance = nullptr;

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

void ClientNetworkManager::Initiate()
{
	myIsServer = false;
	myNetwork = new ClientNetwork();
	myNetworkID = 0;
	__super::Initiate();
	myClients.Init(16);
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

void ClientNetworkManager::HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress)
{
	NetMessagePingReply reply;
	reply.PackMessage();
	myDataSent += reply.myStream.size() * sizeof(char);
	myNetwork->Send(reply.myStream);
}

void ClientNetworkManager::HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress)
{
	myNetworkID = aMessage.myServerID;
	for (unsigned short i : aMessage.myClientsOnServer)
	{
		if (i != myNetworkID)
		{
			myClients.Add(OtherClients(i));
		}
	}
}

void ClientNetworkManager::HandleMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress)
{
	for (OtherClients& client : myClients)
	{
		if (client.myID == aMessage.mySenderID)
		{
			return;
		}
	}
	if (aMessage.mySenderID != myNetworkID)
	{
		myClients.Add(OtherClients(aMessage.mySenderID));
	}
}

void ClientNetworkManager::HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress)
{
	//PostMaster::GetInstance()->SendMessage(PostNetPositionMessage(aMessage.myPosition, aMessage.mySenderID));

	for (OtherClients &client : myClients)
	{
		if (client.myID == aMessage.mySenderID)
		{
			client.myPosition = aMessage.myPosition;
		}
	}
}



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



void ClientNetworkManager::ConnectToServer(const char* aServerIP)
{
	DL_ASSERT_EXP(myIsServer == false, "Tried to Connect to Server from Server... this doesn't seem right.");
	myIsOnline = myNetwork->ConnectToServer(aServerIP);
	char username[256 + 1];
	DWORD username_len = 256 + 1;
	GetUserNameA(username, &username_len);
	AddMessage(NetMessageConnectMessage(username, -1));
}

const CU::GrowingArray<OtherClients>& ClientNetworkManager::GetClients()
{
	return myClients;
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

