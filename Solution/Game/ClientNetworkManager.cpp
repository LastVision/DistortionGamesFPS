#include "stdafx.h"
#include "ClientNetworkManager.h"
#include "ClientNetwork.h"
#include <thread>
#include <PostMaster.h>

#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>

#include <NetworkAddPlayerMessage.h>

#define BUFFERSIZE 512

ClientNetworkManager* ClientNetworkManager::myInstance = nullptr;

ClientNetworkManager::ClientNetworkManager()
{
}

ClientNetworkManager::~ClientNetworkManager()
{
	myMainIsDone = true;
	myReceieveIsDone = true;
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

ClientNetworkManager* ClientNetworkManager::GetInstance()
{
	if (myInstance != nullptr)
	{
		return myInstance;
	}
	DL_ASSERT("Instance were null, did you forget to create the ClientNetworkManager?");
	return nullptr;
}

void ClientNetworkManager::Initiate()
{
	myIsServer = false;
	myNetwork = new ClientNetwork();
	myNetworkID = 0;
	__super::Initiate();
	myClients.Init(16);
}

void ClientNetworkManager::StartNetwork(unsigned int aPortNum)
{
	myNetwork->StartNetwork(aPortNum);
	__super::StartNetwork(aPortNum);
}

void ClientNetworkManager::ReceieveThread()
{
	char buffer[BUFFERSIZE];
	while (myIsRunning == true)
	{
		ZeroMemory(&buffer, BUFFERSIZE);

		std::vector<Buffer> someBuffers;
		myNetwork->Receieve(someBuffers);
		for (Buffer message : someBuffers)
		{
			myReceieveBuffer[myCurrentBuffer ^ 1].Add(message);
		}
		ReceieveIsDone();
		WaitForMain();
		Sleep(1);
	}
}

void ClientNetworkManager::SendThread()
{
	while (myIsRunning == true)
	{
		for (std::vector<char> arr : mySendBuffer[myCurrentSendBuffer])
		{
			myNetwork->Send(arr);
		}

		mySendBuffer[myCurrentSendBuffer].RemoveAll();
		myCurrentSendBuffer ^= 1;
		Sleep(1);
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

void ClientNetworkManager::HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress)
{
	aMessage;
	aSenderAddress;
	NetMessagePingReply reply;
	reply.PackMessage();
	myDataSent += reply.myStream.size() * sizeof(char);
	myNetwork->Send(reply.myStream);
}

void ClientNetworkManager::HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress)
{
	aSenderAddress;
	myNetworkID = aMessage.myServerID;
	if (aMessage.myOtherClientID != myNetworkID)
	{
		myClients.Add(OtherClients(aMessage.myOtherClientID));
		PostMaster::GetInstance()->SendMessage(NetworkAddPlayerMessage(aMessage.myOtherClientID));
	}
}

void ClientNetworkManager::HandleMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress)
{
	aSenderAddress;
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
		PostMaster::GetInstance()->SendMessage(NetworkAddPlayerMessage(aMessage.mySenderID));
	}
}

void ClientNetworkManager::HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress)
{
	aSenderAddress;
	for (OtherClients &client : myClients)
	{
		if (client.myID == aMessage.mySenderID)
		{
			client.myPosition = aMessage.myPosition;
		}
	}
}