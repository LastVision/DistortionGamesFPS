#include "stdafx.h"
#include "ClientNetworkManager.h"
#include "ClientNetwork.h"
#include <thread>
#include <PostMaster.h>

#include <NetMessageImportantReply.h>
#include <NetMessageConnectReply.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageOnHit.h>
#include <NetMessageOnJoin.h>
#include <NetMessageDisconnect.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageOnDeath.h>
#include <NetMessageStartGame.h>

#include <PostMasterNetAddPlayerMessage.h>
#include <PostMasterNetOnHitMessage.h>
#include <PostMasterNetRemovePlayer.h>
#include <PostMasterNetAddEnemyMessage.h>
#include <PostMasterNetSetPositionMessage.h>
#include <PostMasterNetSendPositionMessage.h>
#include <PostMasterNetOnDisconnectMessage.h>
#include <PostMasterNetOnDeathMessage.h>
#include <PostMasterNetStartGameMessage.h>

#define BUFFERSIZE 512

ClientNetworkManager* ClientNetworkManager::myInstance = nullptr;

ClientNetworkManager::ClientNetworkManager()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_SEND_POSITION, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_DISCONNECT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_HIT, this);
}

ClientNetworkManager::~ClientNetworkManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_SEND_POSITION, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ON_DISCONNECT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ON_HIT, this);

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
	myGID = 0;
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
		for (SendBufferMessage arr : mySendBuffer[myCurrentSendBuffer])
		{
			myNetwork->Send(arr.myBuffer);
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
	NetMessageRequestConnect connect = CreateMessage<NetMessageRequestConnect>();
	connect.myName = username;
	connect.myServerID = 0;
	AddMessage(connect);

	myName = username;
}

unsigned int ClientNetworkManager::GetGID() const
{
	return myGID;
}

const CU::GrowingArray<OtherClients>& ClientNetworkManager::GetClients()
{
	return myClients;
}

void ClientNetworkManager::ReceiveMessage(const PostMasterNetSendPositionMessage& aMessage)
{
	NetMessagePosition toSend;
	toSend.mySenderID = myGID;
	toSend.myPosition = aMessage.myPosition;
	toSend.myRotationY = aMessage.myRotationY;
	toSend.myGID = aMessage.myGID;
	AddMessage(toSend);
}

void ClientNetworkManager::ReceiveMessage(const PostMasterNetOnDisconnectMessage&)
{
	NetMessageDisconnect toSend = CreateMessage<NetMessageDisconnect>();
	toSend.mySenderID = myGID;
	toSend.myClientID = myGID;
	AddMessage(toSend);
}

void ClientNetworkManager::ReceiveMessage(const PostMasterNetOnHitMessage& aMessage)
{
	NetMessageOnHit toSend;
	toSend.mySenderID = myGID;
	toSend.myDamage = aMessage.myDamage;
	toSend.myGID = aMessage.myGID;
	AddMessage(toSend);
}

void ClientNetworkManager::DebugPrint()
{ 
	DEBUG_PRINT(myGID);
	DEBUG_PRINT(myName);

	for each (const OtherClients& client in myClients)
	{
		DEBUG_PRINT(client.myID);
		DEBUG_PRINT(client.myName);
	}
}

void ClientNetworkManager::HandleMessage(const NetMessagePingRequest&, const sockaddr_in&)
{
	NetMessagePingReply reply;
	reply.mySenderID = myGID;
	reply.PackMessage();
	myDataSent += reply.myStream.size() * sizeof(char);
	myNetwork->Send(reply.myStream);
}

void ClientNetworkManager::HandleMessage(const NetMessageDisconnect& aMessage, const sockaddr_in&)
{
	if (aMessage.myClientID == myGID)
	{
		MessageBox(NULL, "You have been disconnected!", "Connection Lost!", MB_ICONERROR | MB_OK);
	}
	else
	{
		PostMaster::GetInstance()->SendMessage(PostMasterNetRemovePlayerMessage(aMessage.myClientID));
	}
}

void ClientNetworkManager::HandleMessage(const NetMessageConnectReply& aMessage, const sockaddr_in&)
{
	if (aMessage.myType == NetMessageConnectReply::eType::SUCCESS)
	{
		myGID = aMessage.myGID;
	}
	else
	{
		DL_ASSERT("Failed to connect");
	}
}

void ClientNetworkManager::HandleMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in&)
{
	DL_ASSERT("Should not happen");
	/*if (aMessage.myOtherClientID != myGID)
	{
		myClients.Add(OtherClients(aMessage.myOtherClientID));
	}*/
	//bool needsToBeImplemented = true;
	//PostMaster::GetInstance()->SendMessage(PostMasterNetAddPlayerMessage(aMessage.myOtherClientID));
}

void ClientNetworkManager::HandleMessage(const NetMessageOnJoin& aMessage, const sockaddr_in&)
{
	if (aMessage.mySenderID != myGID)
	{
		myClients.Add(OtherClients(aMessage.myName, aMessage.mySenderID));
		bool needsToBeImplemented = true;
		//PostMaster::GetInstance()->SendMessage(PostMasterNetAddPlayerMessage(static_cast<unsigned short>(aMessage.mySenderID)));
	}
}

void ClientNetworkManager::HandleMessage(const NetMessageAddEnemy& aMessage, const sockaddr_in&)
{
	PostMaster::GetInstance()->SendMessage(PostMasterNetAddEnemyMessage(aMessage.myPosition, aMessage.myGID));
}

void ClientNetworkManager::HandleMessage(const NetMessageOnDeath& aMessage, const sockaddr_in&)
{
	PostMaster::GetInstance()->SendMessage(PostMasterNetOnDeathMessage(aMessage.myGID));
}

void ClientNetworkManager::HandleMessage(const NetMessageStartGame& aMessage, const sockaddr_in&)
{
	PostMaster::GetInstance()->SendMessage(PostMasterNetStartGameMessage(aMessage.myLevelID));
}

void ClientNetworkManager::UpdateImportantMessages(float aDeltaTime)
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
	msg.myMessageType = aBuffer[0];
	msg.mySenders.Init(1);
	ImportantClient server;
	server.myGID = 0;
	server.myNetworkAddress = myNetwork->GetServerAddress();
	server.myName = "server";
	server.myTimer = 0.f;
	server.myHasReplied = false;
	msg.mySenders.Add(server);
	myImportantMessagesBuffer.Add(msg);
}