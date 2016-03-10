#include "stdafx.h"
#include "ClientNetworkManager.h"
#include "ClientNetwork.h"
#include <thread>
#include <PostMaster.h>

#include <NetMessageImportantReply.h>
#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <NetMessageDisconnect.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageOnDeath.h>

#include <NetworkAddPlayerMessage.h>
#include <NetworkRemovePlayer.h>
#include <NetworkAddEnemyMessage.h>
#include <NetworkSetPositionMessage.h>
#include <NetworkSendPositionMessage.h>
#include <NetworkOnDisconnectMessage.h>
#include <NetworkOnDeathMessage.h>
#define BUFFERSIZE 512

ClientNetworkManager* ClientNetworkManager::myInstance = nullptr;

ClientNetworkManager::ClientNetworkManager()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_SEND_POSITION, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_DISCONNECT, this);
}

ClientNetworkManager::~ClientNetworkManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_SEND_POSITION, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ON_DISCONNECT, this);
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
	NetMessageConnectMessage connect = CreateMessage<NetMessageConnectMessage>();
	connect.myName = username;
	connect.myServerID = -1;
	AddMessage(connect);
}

unsigned int ClientNetworkManager::GetNetworkID() const
{
	return myNetworkID;
}

const CU::GrowingArray<OtherClients>& ClientNetworkManager::GetClients()
{
	return myClients;
}

void ClientNetworkManager::ReceiveMessage(const NetworkSendPositionMessage& aMessage)
{
	NetMessagePosition toSend;
	toSend.mySenderID = myNetworkID;
	toSend.myPosition = aMessage.myPosition;
	toSend.myRotationY = aMessage.myRotationY;
	toSend.myNetworkID = aMessage.myNetworkID;
	AddMessage(toSend);
}

void ClientNetworkManager::ReceiveMessage(const NetworkOnDisconnectMessage&)
{
	NetMessageDisconnect toSend = CreateMessage<NetMessageDisconnect>();
	toSend.mySenderID = myNetworkID;
	toSend.myClientID = myNetworkID;
	AddMessage(toSend);
}

void ClientNetworkManager::HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress)
{
	aMessage;
	aSenderAddress;
	NetMessagePingReply reply;
	reply.mySenderID = myNetworkID;
	reply.PackMessage();
	myDataSent += reply.myStream.size() * sizeof(char);
	myNetwork->Send(reply.myStream);
}

void ClientNetworkManager::HandleMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress)
{
	if (CheckIfImportantMessage(aMessage) == true)
	{
		AddMessage(NetMessageImportantReply(aMessage.GetImportantID()));
	}
	aSenderAddress;
	if (aMessage.myClientID == myNetworkID)
	{
		MessageBox(NULL, "You have been disconnected!", "Connection Lost!", MB_ICONERROR | MB_OK);
	}
	else
	{
		PostMaster::GetInstance()->SendMessage(NetworkRemovePlayerMessage(aMessage.myClientID));
	}
}

void ClientNetworkManager::HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress)
{
	
	aSenderAddress;
	myNetworkID = aMessage.myServerID;
	if (aMessage.myOtherClientID != myNetworkID)
	{
		myClients.Add(OtherClients(aMessage.myOtherClientID));
		
	}
	if (CheckIfImportantMessage(aMessage) == true)
	{
		AddMessage(NetMessageImportantReply(aMessage.GetImportantID()));
	}
	PostMaster::GetInstance()->SendMessage(NetworkAddPlayerMessage(aMessage.myOtherClientID));
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
		PostMaster::GetInstance()->SendMessage(NetworkAddPlayerMessage(static_cast<unsigned short>(aMessage.mySenderID)));
		
	}
	if (CheckIfImportantMessage(aMessage) == true)
	{
		AddMessage(NetMessageImportantReply(aMessage.GetImportantID()));
	}
}

void ClientNetworkManager::HandleMessage(const NetMessageAddEnemy& aMessage, const sockaddr_in& aSenderAddress)
{
	if (CheckIfImportantMessage(aMessage) == true)
	{
		AddMessage(NetMessageImportantReply(aMessage.GetImportantID()));
	}
	aSenderAddress;
	PostMaster::GetInstance()->SendMessage(NetworkAddEnemyMessage(aMessage.myPosition, aMessage.myNetworkID));
}

void ClientNetworkManager::HandleMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress)
{
	aSenderAddress;
	if (CheckIfImportantMessage(aMessage) == true)
	{
		AddMessage(NetMessageImportantReply(aMessage.GetImportantID()));
	}
	PostMaster::GetInstance()->SendMessage(NetworkOnDeathMessage(aMessage.myNetworkID));
}

void ClientNetworkManager::UpdateImporantMessages(float aDeltaTime)
{
	for (ImportantMessage& msg : myImportantMessagesBuffer)
	{
		bool finished = true;
		for (ImportantClient& client : msg.mySenders)
		{
			if (client.myHasReplied == false)
			{
				finished = false;
				client.myTimer += aDeltaTime;
				if (client.myTimer >= 1.f)
				{
					client.myTimer = 0.f;
					myNetwork->Send(msg.myData);
				}
			}
		}
		if (finished == true)
		{
			myImportantMessagesBuffer.RemoveCyclic(msg);
		}
	}
}

void ClientNetworkManager::AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID)
{
	ImportantMessage msg;
	msg.myData = aBuffer;
	msg.myImportantID = aImportantID;
	msg.mySenders.Init(1);
	ImportantClient server;
	server.myNetworkID = 0;
	server.myNetworkAddress = myNetwork->GetServerAddress();
	server.myTimer = 0.f;
	server.myHasReplied = false;
	msg.mySenders.Add(server);
	myImportantMessagesBuffer.Add(msg);
}