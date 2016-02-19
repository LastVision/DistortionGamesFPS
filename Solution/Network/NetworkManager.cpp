#include "stdafx.h"

#include "ClientInterface.h"
#include "NetworkManager.h"
#include "NetworkMessageTypes.h"
#include "NetMessageConnectMessage.h"
#include "ServerInterface.h"
#include "Utility.h"
#include <DL_Debug.h>
#include <thread>

#define BUFFERSIZE 512
NetworkManager* NetworkManager::myInstance = nullptr;

void NetworkManager::Activate(bool aIsServer)
{
	myIsServer = aIsServer;

	myNetwork = new ClientInterface();
	if (myIsServer == true)
	{
		delete myNetwork;
		myNetwork = nullptr;
		myNetwork = new ServerInterface();
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
	if (myIsServer)
	{
		myReceieveThread = new std::thread([&]{ReceieveThread(); });
		mySendThread = new std::thread([&]{SendThread(); });
	}
}

void NetworkManager::ConnectToServer()
{
	DL_ASSERT_EXP(myIsServer == false, "Tried to Connect to Server from Server... this doesn't seem right.");
	myNetwork->ConnectToServer("localhost");
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

NetworkManager::NetworkManager()
	: myIsServer(false)
{
}

NetworkManager::~NetworkManager()
{
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
	for (;;)
	{
		ZeroMemory(&buffer, BUFFERSIZE);
		int receievedLength = myNetwork->Receieve(buffer);
		eNetMessageType messageType = ReadType(buffer);

		if (messageType == eNetMessageType::NONE)
		{
			continue;
		}


		switch (messageType)
		{
		case eNetMessageType::ON_CONNECT:
		{
			CNetMessageConnectMessage toUnpack;
			toUnpack.UnPackMessage(buffer, receievedLength);
			std::string connected = toUnpack.myName + " has connected!";
			Utility::PrintEndl(connected,Utility::eCOLOR::LIGHT_GREEN_BACK_BLACK);
				break;
		}
		}

	}
}

eNetMessageType NetworkManager::ReadType(const char* aBuffer)
{
	return static_cast<eNetMessageType>(aBuffer[0]);
}
