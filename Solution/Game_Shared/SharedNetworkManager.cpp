#include "stdafx.h"
#include "SharedNetworkManager.h"

#include <NetMessage.h>
#include <NetMessageImportantReply.h>
#include <NetMessageConnectMessage.h>
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

#include <PostMaster.h>
#include <NetworkOnHitMessage.h>
#include <NetworkSetPositionMessage.h>

#define BUFFERSIZE 512

void SharedNetworkManager::Initiate()
{
	myReceieveBuffer[0].Init(16384);
	myReceieveBuffer[1].Init(16384);
	mySendBuffer[0].Init(16384);
	mySendBuffer[1].Init(16384);
	myImportantMessagesBuffer.Init(16384);
	myImportantID = 0;
	myPingTime = 0.f;
	myReceieveIsDone = true;
	myMainIsDone = true;
}

void SharedNetworkManager::StartNetwork(unsigned int /*aPortNum*/)
{

	myIsRunning = true;
	myReceieveThread = new std::thread([&] {ReceieveThread(); });
	mySendThread = new std::thread([&] {SendThread(); });

#ifdef _DEBUG
	if (myIsServer == true)
	{
		CU::SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Server");
		CU::SetThreadName(mySendThread->get_id(), "Send Thread - Server");
	}
	else
	{
		CU::SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Client");
		CU::SetThreadName(mySendThread->get_id(), "Send Thread - Client");
	}
#endif
}

SharedNetworkManager::SharedNetworkManager()
	: myIsOnline(false)
	, myIsRunning(false)
	, myCurrentBuffer(0)
	, myCurrentSendBuffer(0)
	, myImportantReceivedMessages(64)
{
}

SharedNetworkManager::~SharedNetworkManager()
{

}

void SharedNetworkManager::Update(float aDelta)
{
	SwapBuffer();
	UpdateImportantMessages(aDelta);
	UpdateImportantReceivedMessages(aDelta);
	myPingTime -= aDelta;
	myResponsTime += aDelta;
	if (myPingTime <= 0.f)
	{
		myResponsTime = 0.f;
		AddMessage(NetMessagePingRequest());
		myDataToPrint = myDataSent;
		myDataSent = 0;
		myPingTime = 1.f;
	}
	HandleMessage();
}

void SharedNetworkManager::SwapBuffer()
{
	myReceieveBuffer[myCurrentBuffer].RemoveAll();
	myCurrentBuffer ^= 1;
}

void SharedNetworkManager::ReceieveIsDone()
{
	myReceieveIsDone = true;
}

void SharedNetworkManager::MainIsDone()
{
	myMainIsDone = true;
}

void SharedNetworkManager::WaitForMain()
{
	while (myMainIsDone == false)
	{
		Sleep(1);
	}
	myMainIsDone = false;
}

void SharedNetworkManager::WaitForReceieve()
{
	while (myReceieveIsDone == false)
	{
		Sleep(1);
	}
	myReceieveIsDone = false;
}

eNetMessageType SharedNetworkManager::ReadType(const char* aBuffer)
{
	return static_cast<eNetMessageType>(aBuffer[0]);
}

eNetMessageType SharedNetworkManager::ReadType(const std::vector<char>& aBuffer)
{
	return static_cast<eNetMessageType>(aBuffer[0]);
}

unsigned short SharedNetworkManager::GetResponsTime() const
{
	return static_cast<unsigned short>(myMS);
}

double SharedNetworkManager::GetDataSent() const
{
	return myDataToPrint / 1024;
}

void SharedNetworkManager::AddNetworkMessage(std::vector<char> aBuffer)
{
	if (myIsOnline == true)
	{
		mySendBuffer[myCurrentSendBuffer ^ 1].Add(aBuffer);
	}
}

void SharedNetworkManager::HandleMessage()
{
	for (Buffer& buffer : myReceieveBuffer[myCurrentBuffer])
	{
		eNetMessageType type = ReadType(buffer.myData);
		switch (type)
		{
		case eNetMessageType::IMPORTANT_REPLY:
			UnpackAndHandle(NetMessageImportantReply(), buffer);
			break;
		case eNetMessageType::ON_CONNECT:
			UnpackAndHandle(NetMessageConnectMessage(), buffer);
			break;
		case eNetMessageType::ON_JOIN:
			UnpackAndHandle(NetMessageOnJoin(), buffer);
			break;
		case eNetMessageType::ON_DISCONNECT:
			UnpackAndHandle(NetMessageDisconnect(), buffer);
			break;
		case eNetMessageType::REQUEST_LEVEL:
			UnpackAndHandle(NetMessageRequestLevel(), buffer);
			break;
		case eNetMessageType::REQUEST_START_GAME:
			UnpackAndHandle(NetMessageRequestStartGame(), buffer);
			break;
		case eNetMessageType::PING_REQUEST:
			UnpackAndHandle(NetMessagePingRequest(), buffer);
			break;
		case eNetMessageType::PING_REPLY:
			UnpackAndHandle(NetMessagePingReply(), buffer);
			break;
		case eNetMessageType::POSITION:
			UnpackAndHandle(NetMessagePosition(), buffer);
			break;
		case eNetMessageType::ADD_ENEMY:
			UnpackAndHandle(NetMessageAddEnemy(), buffer);
			break;
		case eNetMessageType::PLAYER_ON_HIT:
		case eNetMessageType::ENEMY_ON_HIT:
			UnpackAndHandle(NetMessageOnHit(), buffer);
			break;
		case eNetMessageType::PLAYER_ON_DEATH:
		case eNetMessageType::ENEMY_ON_DEATH:
			UnpackAndHandle(NetMessageOnDeath(), buffer);
			break;
		default:
			break;
		}
	}
}

void SharedNetworkManager::HandleMessage(const NetMessagePingReply&, const sockaddr_in&)
{
	myMS = myResponsTime * 1000.f;
}

void SharedNetworkManager::HandleMessage(const NetMessageImportantReply& aMessage, const sockaddr_in&)
{
	for (ImportantMessage& msg : myImportantMessagesBuffer)
	{
		if (aMessage.myImportantID == msg.myImportantID)
		{
			for (ImportantClient& client : msg.mySenders)
			{
				if (aMessage.mySenderID == client.myGID)
				{
					client.myHasReplied = true;
					return;
				}
			}
		}
	}
}
void SharedNetworkManager::HandleMessage(const NetMessageConnectMessage&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessageDisconnect&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessageRequestLevel&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessagePingRequest&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessageOnJoin&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in&) 
{
	PostMaster::GetInstance()->SendMessage(NetworkSetPositionMessage(aMessage.myPosition, aMessage.myRotationY, aMessage.myGID)); 
}
void SharedNetworkManager::HandleMessage(const NetMessageAddEnemy&, const sockaddr_in&){}
void SharedNetworkManager::HandleMessage(const NetMessageOnHit& aMessage, const sockaddr_in&)
{ 
	PostMaster::GetInstance()->SendMessage(NetworkOnHitMessage(aMessage.myDamage, aMessage.myGID)); 
}
void SharedNetworkManager::HandleMessage(const NetMessageOnDeath&, const sockaddr_in&) {}

void SharedNetworkManager::HandleMessage(const NetMessageRequestStartGame&, const sockaddr_in&) {}

bool SharedNetworkManager::AlreadyReceived(const NetMessage& aMessage)
{
	if (aMessage.GetIsImportant() == false)
	{
		return false;
	}

	for (int i = 0; i < myImportantReceivedMessages.Size(); ++i)
	{
		if (aMessage.GetImportantID() == myImportantReceivedMessages[i].myImportantID
			&& aMessage.mySenderID == myImportantReceivedMessages[i].mySenderGID)
		{
			return true;
		}
	}

	myImportantReceivedMessages.Add(ImportantReceivedMessage(aMessage.GetImportantID(), aMessage.mySenderID));
	
	return false;
}

void SharedNetworkManager::UpdateImportantReceivedMessages(float aDelta)
{
	for (int i = myImportantReceivedMessages.Size() - 1; i >= 0; --i)
	{
		myImportantReceivedMessages[i].myTimer += aDelta;

		if (myImportantReceivedMessages[i].myTimer > 5.f)
		{
			myImportantReceivedMessages.RemoveCyclicAtIndex(i);
		}
	}
}