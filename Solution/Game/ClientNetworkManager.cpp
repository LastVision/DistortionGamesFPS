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
#include <NetMessageOnDeath.h>
#include <NetMessageLoadLevel.h>

#include <TimerManager.h>

#define BUFFERSIZE 512

ClientNetworkManager::ClientNetworkManager()
{
}

ClientNetworkManager::~ClientNetworkManager()
{
	UnSubscribe(eNetMessageType::CONNECT_REPLY, this);
	UnSubscribe(eNetMessageType::ON_CONNECT, this);
	UnSubscribe(eNetMessageType::ON_DISCONNECT, this);
	UnSubscribe(eNetMessageType::ON_JOIN, this);
	UnSubscribe(eNetMessageType::PING_REQUEST, this);
	UnSubscribe(eNetMessageType::PING_REPLY, this);

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

	if (myPingThread != nullptr)
	{
		myPingThread->join();
		SAFE_DELETE(myPingThread);
	}


	delete myNetwork;
	myNetwork = nullptr;

	SAFE_DELETE(myTimeManager);
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
		return static_cast<ClientNetworkManager*>(myInstance);
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

	myTimeManager = new CU::TimerManager();

	Subscribe(eNetMessageType::CONNECT_REPLY, this);
	Subscribe(eNetMessageType::ON_CONNECT, this);
	Subscribe(eNetMessageType::ON_DISCONNECT, this);
	Subscribe(eNetMessageType::ON_JOIN, this);
	Subscribe(eNetMessageType::PING_REQUEST, this);
	Subscribe(eNetMessageType::PING_REPLY, this);

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
			if (arr.myTargetID == UINT_MAX)
			{
				myNetwork->Send(arr.myBuffer, arr.myTargetAddress);
			}
			else
			{
				myNetwork->Send(arr.myBuffer);
			}
		}

		mySendBuffer[myCurrentSendBuffer].RemoveAll();
		myCurrentSendBuffer ^= 1;
		Sleep(1);
	}
}

void ClientNetworkManager::PingThread()
{
	while (myIsRunning == true)
	{
		if (myIsOnline == true)
		{
			NetMessagePingRequest toSend;
			toSend.PackMessage();
			myNetwork->Send(toSend.myStream);

			if (myHasSent == false)
			{
				myCurrentTimeStamp = myTimeManager->GetMasterTimer().GetTime().GetMilliseconds();
				myHasSent = true;
			}

			Sleep(1000);
		}
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

	AddMessage(NetMessageRequestConnect(username, 0));
}

unsigned int ClientNetworkManager::GetGID() const
{
	return myGID;
}

const CU::GrowingArray<OtherClients>& ClientNetworkManager::GetClients()
{
	return myClients;
}

void ClientNetworkManager::Update(float aDeltaTime)
{
	__super::Update(aDeltaTime);
	myTimeManager->Update();
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

void ClientNetworkManager::ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in&)
{
	if (aMessage.myClientID == myGID)
	{
		MessageBox(NULL, "You have been disconnected!", "Connection Lost!", MB_ICONERROR | MB_OK);
	}
}

void ClientNetworkManager::ReceiveNetworkMessage(const NetMessagePingRequest&, const sockaddr_in&)
{
	NetMessagePingReply toSend;
	toSend.mySenderID = myGID;
	toSend.PackMessage();
	myNetwork->Send(toSend.myStream);
}

void ClientNetworkManager::ReceiveNetworkMessage(const NetMessageConnectReply& aMessage, const sockaddr_in&)
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

void ClientNetworkManager::ReceiveNetworkMessage(const NetMessageRequestConnect&, const sockaddr_in&)
{
	DL_ASSERT("Should not happen");
}

void ClientNetworkManager::ReceiveNetworkMessage(const NetMessageOnJoin& aMessage, const sockaddr_in&)
{
	if (aMessage.myOtherClientID != myGID)
	{
		myClients.Add(OtherClients(aMessage.myName, aMessage.myOtherClientID));
	}
}

void ClientNetworkManager::ReceiveNetworkMessage(const NetMessagePingReply&, const sockaddr_in&)
{
	unsigned long long old = myCurrentTimeStamp;
	myCurrentTimeStamp = myTimeManager->GetMasterTimer().GetTime().GetMilliseconds();
	myMS = (myCurrentTimeStamp - old);
	myHasSent = false;
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