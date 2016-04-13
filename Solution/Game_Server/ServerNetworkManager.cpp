#include "stdafx.h"
#include "ServerNetworkManager.h"
#include "ServerNetwork.h"
#include <thread>
#include <Utility.h>
#include <PostMaster.h>
#include <ConvertNetworkEnumToString.h>

#include <NetMessageImportantReply.h>
#include <NetMessageConnectReply.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageOnJoin.h>
#include <NetMessageDisconnect.h>
#include <NetMessageSetLevel.h>
#include <NetMessageRequestStartLevel.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>
#include <NetMessageOnHit.h>
#include <NetMessageOnDeath.h>
#include <NetMessageLevelLoaded.h>

#include <TimerManager.h>

#define BUFFERSIZE 512
#define RECONNECT_ATTEMPTS 10

ServerNetworkManager::ServerNetworkManager()
	: myAllowNewConnections(false)
	, myPingTime(1.f)
	, myStopPingFlag(false)
{
}

ServerNetworkManager::~ServerNetworkManager()
{

	UnSubscribe(eNetMessageType::POSITION, this);
	UnSubscribe(eNetMessageType::PING_REPLY, this);
	UnSubscribe(eNetMessageType::PING_REQUEST, this);
	UnSubscribe(eNetMessageType::ON_CONNECT, this);
	UnSubscribe(eNetMessageType::ON_DISCONNECT, this);

	myMainIsDone = true;
	myReceieveIsDone = true;
	myIsRunning = false;
	if (myReceieveThread != nullptr)
	{
		myReceieveThread->join();
		SAFE_DELETE(myReceieveThread);
	}

	if (mySendThread != nullptr)
	{
		mySendThread->join();
		SAFE_DELETE(mySendThread);
	}

	if (myPingThread != nullptr)
	{
		myPingThread->join();
		SAFE_DELETE(myPingThread);
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
	myTimeManager = new CU::TimerManager();
	Subscribe(eNetMessageType::POSITION, this);
	Subscribe(eNetMessageType::PING_REPLY, this);
	Subscribe(eNetMessageType::PING_REQUEST, this);
	Subscribe(eNetMessageType::ON_CONNECT, this);
	Subscribe(eNetMessageType::ON_DISCONNECT, this);
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
		return static_cast<ServerNetworkManager*>(myInstance);
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
	std::vector<Buffer> someBuffers;
	while (myIsRunning == true)
	{
		ZeroMemory(&buffer, BUFFERSIZE);
		myNetwork->Receieve(someBuffers);

		if (someBuffers.size() == 0)
		{
			WSAGetLastError();
		}
		for (Buffer message : someBuffers)
		{
			if (message.myLength >= 17)
			{
				NetMessage toDeserialize;
				toDeserialize.DeSerializeFirst(message.myData);
				if (toDeserialize.myGameID == myGameIdentifier)
				{
					myReceieveBuffer[myCurrentBuffer ^ 1].Add(message);
				}
			}
		}
		ReceieveIsDone();
		WaitForMain();
		someBuffers.clear();
		Sleep(1);

	}
}

void ServerNetworkManager::SendThread()
{
	while (myIsRunning == true)
	{
		for (SendBufferMessage arr : mySendBuffer[myCurrentSendBuffer])
		{
			if (arr.myBuffer.size() == 0)
			{
				continue;
			}
			if (arr.myTargetID == UINT_MAX)
			{
				myNetwork->Send(arr.myBuffer, arr.myTargetAddress);
			}
			else if (arr.myTargetID == 0)
			{
				for (Connection& connection : myClients)
				{
					if (connection.myID != static_cast<unsigned int>(arr.myBuffer[9]))
					{
						myNetwork->Send(arr.myBuffer, connection.myAddress);
					}
				}
			}
			else
			{
				for (Connection& connection : myClients)
				{
					if (connection.myID == arr.myTargetID && connection.myID != static_cast<unsigned int>(arr.myBuffer[9]))
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

void ServerNetworkManager::PingThread()
{
	while (myIsRunning == true)
	{
		if (myClients.Size() > 0)
		{
			NetMessagePingRequest toSend;
			toSend.PackMessage();
			for (Connection& connection : myClients)
			{
				myNetwork->Send(toSend.myStream, connection.myAddress);
			}

			if (myHasSent == false)
			{
				myResponsTime = 0.f;
			}

			Sleep(1000);
		}
		Sleep(1);
	}
}

void ServerNetworkManager::CreateConnection(const std::string& aName, const sockaddr_in& aSender)
{
	if (myAllowNewConnections == false)
	{
		AddMessage(NetMessageConnectReply(NetMessageConnectReply::eType::FAIL), aSender);
		return;
	}

	if (myClients.Size() > 1)
	{
		AddMessage(NetMessageConnectReply(NetMessageConnectReply::eType::FAIL), aSender);
		return;
	}

	

	/*NetMessageConnectReply connectReply(NetMessageConnectReply::eType::SUCCESS, myIDCount);
	connectReply.PackMessage();
	myNetwork->Send(connectReply.myStream, aSender);*/

	for (Connection& connection : myClients)
	{
		if (connection.myAddress.sin_addr.S_un.S_addr == aSender.sin_addr.S_un.S_addr) //._.
		{
			Utility::Printf("User already connected!", (DARK_RED_BACK | WHITE_TEXT));
			return;
		}
	}
	myIDCount = 1;
	for (short i = 1; i <= 2; ++i)
	{
		bool idExist = false;
		for each (const Connection& connection in myClients)
		{
			if (connection.myID == static_cast<unsigned int>(i))
			{
				idExist = true;
			}
		}
		if (idExist == false)
		{
			myIDCount = i;
			break;
		}
	}
	AddMessage(NetMessageConnectReply(NetMessageConnectReply::eType::SUCCESS, myIDCount), aSender);
	Sleep(200);
	Connection newConnection;
	newConnection.myAddress = aSender;
	newConnection.myID = myIDCount;
	newConnection.myName = aName;
	newConnection.myPingCount = 0;
	newConnection.myIsConnected = true;
	myClients.Add(newConnection);
	myNames[aName] = 1;

	//AddMessage(NetMessageConnectReply(NetMessageConnectReply::eType::SUCCESS, myIDCount), newConnection.myID);

	for (Connection& connection : myClients)
	{
		if (connection.myID == newConnection.myID) continue;
		AddMessage(NetMessageOnJoin(connection.myName, connection.myID), newConnection.myID);
	}

	std::string conn(aName + " connected to the server!");
	Utility::Printf(conn, LIGHT_GREEN_TEXT, true);

	AddMessage(NetMessageOnJoin(aName, myIDCount));

	if (myClients.Size() >= 2)
	{
		myAllowNewConnections = false;
	}
}

void ServerNetworkManager::Update(float aDelta)
{
	__super::Update(aDelta);
	myTimeManager->Update();
	myCurrentTimeStamp = myTimeManager->GetMasterTimer().GetTime().GetMilliseconds();
	myPingTime -= aDelta;
	if (myPingTime < 0.f)
	{
		if (myStopPingFlag == false)
		{
			for (Connection& clients : myClients)
			{
				clients.myPingCount++;
				if (clients.myPingCount > RECONNECT_ATTEMPTS)
				{
					DisconnectConnection(clients);
				}
			}
		}
		myPingTime = 1.f;
	}
}

void ServerNetworkManager::DisconnectConnection(const Connection& aConnection)
{
	NetMessageDisconnect onDisconnect = NetMessageDisconnect(aConnection.myID);
	onDisconnect.PackMessage();
	myNetwork->Send(onDisconnect.myStream, aConnection.myAddress);

	std::string msg(aConnection.myName + " disconnected from server!");
	Utility::Printf(msg, LIGHT_BLUE_TEXT, true);

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
			myClients.GetLast().myID = aConnection.myID;
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

					std::string resend = "Sending important message " + std::to_string(msg.myImportantID) + " of message type "
						+ ConvertNetworkEnumToString(static_cast<eNetMessageType>(msg.myMessageType)) + " to client id " + std::to_string(client.myGID) + " - " + client.myName;
					Utility::Printf(resend, AQUA_TEXT, true);
					myNetwork->Send(msg.myData, client.myNetworkAddress);
				}
			}
		}
		if (finished == true)
		{
			std::string resend = "All client has replied to the message id " + std::to_string(msg.myImportantID)
				+ " of message type " + ConvertNetworkEnumToString(static_cast<eNetMessageType>(msg.myMessageType));
			Utility::Printf(resend, YELLOW_TEXT, true);
			myImportantMessagesBuffer.RemoveCyclic(msg);
		}
	}
}

void ServerNetworkManager::AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID)
{
	if (myClients.Size() > 0)
	{
		ImportantMessage msg;
		msg.myData = aBuffer;
		msg.myImportantID = aImportantID;
		msg.myMessageType = aBuffer[4];
		msg.mySenders.Init(myClients.Size());
		for (const Connection& c : myClients)
		{
			if (aBuffer[13] == 0 || aBuffer[13] == static_cast<int>(c.myID))
			{
				ImportantClient client;
				client.myGID = c.myID;
				client.myNetworkAddress = c.myAddress;
				client.myName = c.myName;
				client.myTimer = 0.f;
				client.myHasReplied = false;
				msg.mySenders.Add(client);
			}
		}
		myImportantMessagesBuffer.Add(msg);
	}
}

void ServerNetworkManager::AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID, const sockaddr_in& aTargetAddress)
{
		ImportantMessage msg;
		msg.myData = aBuffer;
		msg.myImportantID = aImportantID;
		msg.myMessageType = aBuffer[4];
		msg.mySenders.Init(16);

			ImportantClient client;
			client.myGID = UINT_MAX;
			client.myNetworkAddress = aTargetAddress;
			client.myName = inet_ntoa(aTargetAddress.sin_addr);
			client.myTimer = 0.f;
			client.myHasReplied = false;
			msg.mySenders.Add(client);

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
	if (AlreadyReceived(aMessage) == false)
	{
		if (myAllowNewConnections == false)
		{
			/*NetMessageConnectReply connectReply(NetMessageConnectReply::eType::FAIL);
			connectReply.PackMessage();
			myNetwork->Send(connectReply.myStream, aSenderAddress);*/
			AddMessage(NetMessageConnectReply(NetMessageConnectReply::eType::FAIL), aSenderAddress);
			//Bounce
		}
	}
}

void ServerNetworkManager::ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in&)
{
	Utility::Printf("Client requested to close server.", eConsoleColor::WHITE_TEXT);
	if (aMessage.myClientID == 0)
	{
		DisconnectAll();
	}
	else
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
}

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

void ServerNetworkManager::ReceiveNetworkMessage(const NetMessagePosition& aMessage, const sockaddr_in&)
{
	AddMessage(aMessage);
}

void ServerNetworkManager::ReceiveNetworkMessage(const NetMessagePingRequest&, const sockaddr_in&)
{
	NetMessagePingReply toSend;
	toSend.PackMessage();
	for (Connection& connection : myClients)
	{
		myNetwork->Send(toSend.myStream, connection.myAddress);
	}
}

const std::string& ServerNetworkManager::GetIP() const
{
	return myNetwork->GetIP();
}

void ServerNetworkManager::DisconnectAll()
{
	for (int i = 0; i < myClients.Size(); ++i)
	{
		DisconnectConnection(myClients[i]);
	}
	myIDCount = 0;
}