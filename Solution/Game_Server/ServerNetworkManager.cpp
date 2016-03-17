#include "stdafx.h"
#include "ServerNetworkManager.h"
#include "ServerNetwork.h"
#include <thread>
#include <Utility.h>
#include <PostMaster.h>

#include <NetMessageImportantReply.h>
#include <NetMessageConnectReply.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageOnJoin.h>
#include <NetMessageDisconnect.h>
#include <NetMessageRequestLevel.h>
#include <NetMessageRequestStartGame.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageOnHit.h>
#include <NetMessageOnDeath.h>
#include <NetMessageLevelLoaded.h>

#define BUFFERSIZE 512
#define RECONNECT_ATTEMPTS 10000

ServerNetworkManager* ServerNetworkManager::myInstance = nullptr;

ServerNetworkManager::ServerNetworkManager()
	: myAllowNewConnections(false)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_SEND_POSITION, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_DEATH, this);
}

ServerNetworkManager::~ServerNetworkManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_SEND_POSITION, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ON_DEATH, this);

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
	myGID = 0;
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
	myNetwork->PrintStatus();
	__super::StartNetwork(aPortNum);
	myIsOnline = true;
}

bool ServerNetworkManager::ListContainsAllClients(const CU::GrowingArray<unsigned int>& someClientIDs) const
{
	if (myClients.Size() != someClientIDs.Size())
	{
		return false;
	}

	for each (const Connection& connection in myClients)
	{
		if (someClientIDs.Find(connection.myID) == someClientIDs.FoundNone)
		{
			return false;
		}
	}

	return true;
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
		for (SendBufferMessage arr : mySendBuffer[myCurrentSendBuffer])
		{
			if (arr.myTargetID == 0)
			{
				for (Connection& connection : myClients)
				{
					if (connection.myID != static_cast<unsigned int>(arr.myBuffer[5]))
					{
						myNetwork->Send(arr.myBuffer, connection.myAddress);
					}
				}
			}
			else 
			{
				for (Connection& connection : myClients)
				{
					if (connection.myID == arr.myTargetID && connection.myID != static_cast<unsigned int>(arr.myBuffer[5]))
					{
						myNetwork->Send(arr.myBuffer, connection.myAddress);
						break;
					}
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
	myIDCount++;
	NetMessageConnectReply connectReply(NetMessageConnectReply::eType::SUCCESS, myIDCount);
	connectReply.PackMessage();
	myNetwork->Send(connectReply.myStream, aSender);

	Sleep(200);
	//for (Connection& connection : myClients)
	//{
	//	if (connection.myAddress.sin_addr.S_un.S_addr == aSender.sin_addr.S_un.S_addr) //._.
	//	{
	//		Utility::PrintEndl("User already connected!", (DARK_RED_BACK | WHITE_TEXT));
	//		return;
	//	}
	//}

	for (Connection& connection : myClients)
	{
		NetMessageOnJoin msg(connection.myName, connection.myID);
		msg.PackMessage();
		myNetwork->Send(msg.myStream, aSender);
	}

	
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
	
	NetMessageOnJoin onJoin(aName, myIDCount);
	AddMessage(onJoin);
}

void ServerNetworkManager::DisconnectConnection(const Connection& aConnection)
{
	NetMessageDisconnect onDisconnect = NetMessageDisconnect(aConnection.myID);
	onDisconnect.PackMessage();
	myNetwork->Send(onDisconnect.myStream, aConnection.myAddress);
	
	std::string msg(aConnection.myName + " disconnected from server!");
	Utility::PrintEndl(msg, LIGHT_BLUE_TEXT);

	//auto reply on all important messages
	for (ImportantMessage& impMsg : myImportantMessagesBuffer)
	{
		for (ImportantClient& client : impMsg.mySenders)
		{
			if (client.myGID == aConnection.myID)
			{
				client.myHasReplied = true;
				break;
			}
		}
	}

	for (int i = 0; i < myClients.Size(); ++i)
	{
		if (aConnection.myID == myClients[i].myID)
		{
			myClients.RemoveCyclicAtIndex(i);
			break;
		}
	}
	//Send to all other clients which client has been disconnected
	AddMessage(NetMessageDisconnect(aConnection.myID));
}

void ServerNetworkManager::UpdateImportantMessages(float aDeltaTime)
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
					
					std::string resend = "Sending important message " + std::to_string(msg.myImportantID) + " of message type id " 
						+ std::to_string(msg.myMessageType) + " to client id " + std::to_string(client.myGID) + " - " + client.myName;
					Utility::PrintEndl(resend, AQUA_TEXT);
					myNetwork->Send(msg.myData, client.myNetworkAddress);
				}
			}
		}
		if (finished == true)
		{
			std::string resend = "All client has replied to the message id " + std::to_string(msg.myImportantID) 
				+ " of message type id " + std::to_string(msg.myMessageType);
			Utility::PrintEndl(resend, YELLOW_TEXT);
			myImportantMessagesBuffer.RemoveCyclic(msg);
		}
	}
}

void ServerNetworkManager::AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID)
{
	ImportantMessage msg;
	msg.myData = aBuffer;
	msg.myImportantID = aImportantID;
	msg.myMessageType = aBuffer[0];
	msg.mySenders.Init(myClients.Size());
	for (Connection c : myClients)
	{
		ImportantClient client;
		client.myGID = c.myID;
		client.myNetworkAddress = c.myAddress;
		client.myName = c.myName;
		client.myTimer = 0.f;
		client.myHasReplied = false;
		msg.mySenders.Add(client);
	}
	myImportantMessagesBuffer.Add(msg);
}

void ServerNetworkManager::ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress)
{
	if (CheckIfImportantMessage(aMessage) == true)
	{
		NetMessageImportantReply toReply(aMessage.GetImportantID());
		toReply.PackMessage();
		
		myNetwork->Send(toReply.myStream, aSenderAddress);
	}

	if(myAllowNewConnections == false)
	{
		NetMessageConnectReply connectReply(NetMessageConnectReply::eType::FAIL);
		connectReply.PackMessage();
		myNetwork->Send(connectReply.myStream, aSenderAddress);
		//Bounce
	}
	//CreateConnection(aMessage.myName, aSenderAddress);
}

void ServerNetworkManager::ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in&)
{
	for (Connection c : myClients)
	{
		if (c.myID == aMessage.myClientID)
		{
			DisconnectConnection(c);
			break;
		}
	}
}

//void ServerNetworkManager::HandleMessage(const NetMessageRequestStartGame&, const sockaddr_in&)
//{
//	PostMaster::GetInstance()->SendMessage(PostMasterNetRequestStartGameMessage());
//}

void ServerNetworkManager::ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in&)
{
	for (Connection& c : myClients)
	{
		if (c.myID == aMessage.mySenderID)
		{
			c.myPingCount = 0;
			break;
		}
	}
}

void ServerNetworkManager::ReceiveNetworkMessage(const NetMessagePingRequest& aMessage, const sockaddr_in&)
{
	AddMessage(NetMessagePingReply(), aMessage.mySenderID);
	for (Connection& c : myClients)
	{
		c.myPingCount++;
		if (c.myPingCount > RECONNECT_ATTEMPTS)
		{
			DisconnectConnection(c);
		}
	}
}

/*
void ServerNetworkManager::HandleMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in&)
{
	PostMaster::GetInstance()->SendMessage(PostMasterNetLevelLoadedMessage(aMessage.mySenderID));
}

void ServerNetworkManager::ReceiveMessage(const PostMasterNetAddEnemyMessage& aMessage)
{
	NetMessageAddEnemy toSend = CreateMessage<NetMessageAddEnemy>();
	toSend.myPosition = aMessage.myPosition;
	toSend.myGID = aMessage.myGID;
	toSend.PackMessage();
	myNetwork->Send(toSend.myStream, aMessage.myAddress);
}

void ServerNetworkManager::ReceiveMessage(const PostMasterNetSendPositionMessage& aMessage)
{
	NetMessagePosition toSend;
	toSend.mySenderID = static_cast<short>(aMessage.myGID);
	toSend.myPosition = aMessage.myPosition;
	toSend.myRotationY = aMessage.myRotationY;
	toSend.myGID = aMessage.myGID;
	AddMessage(toSend);
}

void ServerNetworkManager::ReceiveMessage(const PostMasterNetOnDeathMessage& aMessage)
{
	NetMessageOnDeath toSend = CreateMessage<NetMessageOnDeath>();
	toSend.mySenderID = 0;
	toSend.myGID = aMessage.myGID;
	AddMessage(toSend);
}*/