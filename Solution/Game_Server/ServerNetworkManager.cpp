#include "stdafx.h"
#include "ServerNetworkManager.h"
#include "ServerNetwork.h"
#include <thread>
#include <Utility.h>
#include <PostMaster.h>

#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <NetMessageDisconnect.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageOnHit.h>

#include <NetworkAddPlayerMessage.h>
#include <NetworkAddEnemyMessage.h>
#include <NetworkSendPositionMessage.h>
#include <NetworkSetPositionMessage.h>
#include <NetworkOnHitMessage.h>
#define BUFFERSIZE 512
#define NUM_TRY_BEFORE_DISCONNECT 10

ServerNetworkManager* ServerNetworkManager::myInstance = nullptr;

ServerNetworkManager::ServerNetworkManager()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_SEND_POSITION, this);
}

ServerNetworkManager::~ServerNetworkManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_SEND_POSITION, this);

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

void ServerNetworkManager::StartNetwork(unsigned int aPortNum)
{
	myNetwork->StartServer(aPortNum);
	__super::StartNetwork(aPortNum);
	myIsOnline = true;
}

void ServerNetworkManager::ReceieveThread()
{
	char buffer[BUFFERSIZE];
	while (myIsRunning == true)
	{
		ZeroMemory(&buffer, BUFFERSIZE);

		std::vector<Buffer> someBuffers;
		myNetwork->Receieve(someBuffers);

		if (someBuffers.size() == 0)
		{
			WSAGetLastError();
		}
		for (Buffer message : someBuffers)
		{
			//Utility::PrintEndl("Server receieved a message.", LIGHT_GREEN_TEXT);
			myReceieveBuffer[myCurrentBuffer ^ 1].Add(message);
		}
		ReceieveIsDone();
		WaitForMain();
		Sleep(1);

	}
}

void ServerNetworkManager::SendThread()
{
	while (myIsRunning == true)
	{
		for (std::vector<char> arr : mySendBuffer[myCurrentSendBuffer])
		{
			for (Connection& connection : myClients)
			{
				if (connection.myID != short(arr[5]))
				{
					myNetwork->Send(arr, connection.myAddress);
				}
			}
		}
		mySendBuffer[myCurrentSendBuffer].RemoveAll();
		myCurrentSendBuffer ^= 1;
		Sleep(1);
	}
}

void ServerNetworkManager::CreateConnection(const std::string& aName, const sockaddr_in& aSender)
{
	//for (Connection& connection : myClients)
	//{
	//	if (connection.myAddress.sin_addr.S_un.S_addr == aSender.sin_addr.S_un.S_addr) //._.
	//	{
	//		Utility::PrintEndl("User already connected!", (DARK_RED_BACK | WHITE_TEXT));
	//		return;
	//	}
	//}
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

	//	onConnect.myName = aName;
	//	onConnect.myServerID = myIDCount;

	for (Connection& connection : myClients)
	{
		NetMessageConnectMessage onConnect(aName, myIDCount, connection.myID);
		onConnect.PackMessage();
		myNetwork->Send(onConnect.myStream, newConnection.myAddress);
	}
	AddMessage(NetMessageOnJoin(newConnection.myID));
	PostMaster::GetInstance()->SendMessage(NetworkAddPlayerMessage(myIDCount, newConnection.myAddress));
}

void ServerNetworkManager::DisconnectConnection(const Connection& aConnection)
{
	//send disconnect message to the client
	NetMessageDisconnect disconnect(aConnection.myID);
	disconnect.PackMessage();
	myNetwork->Send(disconnect.myStream, aConnection.myAddress);
	AddMessage(disconnect);
	//remove the disconnected client from server
	std::string msg(aConnection.myName + " disconnected from server!");
	Utility::PrintEndl(msg, LIGHT_BLUE_TEXT);
	for (int i = 0; i < myClients.Size(); ++i)
	{
		if (aConnection.myID == myClients[i].myID)
		{
			myClients.RemoveCyclicAtIndex(i);
			break;
		}
	}
}

void ServerNetworkManager::HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress)
{
	CreateConnection(aMessage.myName, aSenderAddress);
}

void ServerNetworkManager::HandleMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress)
{
	__super::HandleMessage(aMessage, aSenderAddress);
	for (Connection& c : myClients)
	{
		if (c.myID == aMessage.mySenderID)
		{
			c.myPingCount = 0;
			break;
		}
	}
}

void ServerNetworkManager::HandleMessage(const NetMessagePingRequest&, const sockaddr_in& aSenderAddress)
{
	NetMessagePingReply reply;
	reply.PackMessage();

	myNetwork->Send(reply.myStream, aSenderAddress);
	for (Connection& c : myClients)
	{
		c.myPingCount++;
		if (c.myPingCount > NUM_TRY_BEFORE_DISCONNECT)
		{
			DisconnectConnection(c);
		}
	}
}

void ServerNetworkManager::HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in&)
{
	PostMaster::GetInstance()->SendMessage(NetworkSetPositionMessage(aMessage.myPosition, aMessage.myNetworkID));
}

void ServerNetworkManager::HandleMessage(const NetMessageOnHit& aMessage, const sockaddr_in&)
{
	PostMaster::GetInstance()->SendMessage(NetworkOnHitMessage(aMessage.myDamage, aMessage.myNetworkID));
}

void ServerNetworkManager::ReceiveMessage(const NetworkAddEnemyMessage& aMessage)
{
	NetMessageAddEnemy toSend;
	toSend.myPosition = aMessage.myPosition;
	toSend.myNetworkID = aMessage.myNetworkID;
	toSend.PackMessage();
	myNetwork->Send(toSend.myStream, aMessage.myAddress);
}

void ServerNetworkManager::ReceiveMessage(const NetworkSendPositionMessage& aMessage)
{
	NetMessagePosition toSend;
	toSend.mySenderID = (short)aMessage.myNetworkID;
	toSend.myPosition = aMessage.myPosition;
	toSend.myNetworkID = aMessage.myNetworkID;
	AddMessage(toSend);
}

