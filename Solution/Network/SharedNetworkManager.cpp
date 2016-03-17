#include "stdafx.h"
#include "SharedNetworkManager.h"

#include "NetMessage.h"
#include "NetMessageImportantReply.h"
#include "NetMessageConnectReply.h"
#include "NetMessageRequestConnect.h"
#include "NetMessageOnJoin.h"
#include "NetMessageDisconnect.h"
#include "NetMessageRequestLevel.h"
#include "NetMessageRequestStartGame.h"
#include "NetMessagePingRequest.h"
#include "NetMessagePingReply.h"
#include "NetMessagePosition.h"
#include "NetMessageAddEnemy.h"
#include "NetMessageOnHit.h"
#include "NetMessageOnDeath.h"
#include "NetMessageStartGame.h"
#include "NetMessageLevelLoaded.h"

#define BUFFERSIZE 512

SharedNetworkManager* SharedNetworkManager::myInstance = nullptr;

SharedNetworkManager* SharedNetworkManager::GetInstance()
{
	if (myInstance != nullptr)
	{
		return myInstance;
	}
	DL_ASSERT("Instance were null, did you forget to create the NetworkManager?");
	return nullptr;
}

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
	for (int i = 0; i < static_cast<int>(eNetMessageType::_COUNT); ++i)
	{
		mySubscribers[i].Init(64);
}
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
	for (int i = 0; i < static_cast<int>(eNetMessageType::_COUNT); ++i)
	{
		if (mySubscribers[i].Size() > 0)
		{
			DL_DEBUG("Subscriber not unsubscribed at index %i", i);
			DL_ASSERT("Subscriber not unsubscribed at NetworkManager-Destroy.");
}
		mySubscribers[i].RemoveAll();
	}
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

void SharedNetworkManager::Subscribe(const eNetMessageType aMessageType, NetworkSubscriber* aSubscriber)
{
#ifdef _DEBUG
	CU::GrowingArray<NetworkSubscriberInfo>& subscribers = mySubscribers[static_cast<int>(aMessageType)];
	for (int i = 0; i < subscribers.Size(); ++i)
	{
		DL_ASSERT_EXP(subscribers[i].myNetworkSubscriber != aSubscriber, "Tried to add the same subscriber to the same network message twice.");
	}
#endif

	NetworkSubscriberInfo newSubsciber;
	newSubsciber.myNetworkSubscriber = aSubscriber;
	mySubscribers[static_cast<int>(aMessageType)].Add(newSubsciber);
}

void SharedNetworkManager::UnSubscribe(const eNetMessageType aMessageType, NetworkSubscriber* aSubscriber)
{
	CU::GrowingArray<NetworkSubscriberInfo>& subscribers = mySubscribers[static_cast<int>(aMessageType)];

	for (int i = 0; i < subscribers.Size(); ++i)
	{
		if (subscribers[i].myNetworkSubscriber == aSubscriber)
		{
			subscribers.RemoveCyclicAtIndex(i);
			break;
		}
	}
}

void SharedNetworkManager::UnSubscribe(NetworkSubscriber* aSubscriber)
{
	for (int i = 0; i < static_cast<int>(eNetMessageType::_COUNT); ++i)
	{
		CU::GrowingArray<NetworkSubscriberInfo, int>& subscribers = mySubscribers[i];

		for (int j = 0; j < subscribers.Size(); ++j)
		{
			if (subscribers[j].myNetworkSubscriber == aSubscriber)
			{
				subscribers.RemoveCyclicAtIndex(j);
				break;
			}
		}
	}
}

bool SharedNetworkManager::IsSubscribed(const eNetMessageType aMessageType, NetworkSubscriber* aSubscriber)
{
	CU::GrowingArray<NetworkSubscriberInfo>& subscribers = mySubscribers[static_cast<int>(aMessageType)];

	for (int i = 0; i < subscribers.Size(); ++i)
	{
		if (subscribers[i].myNetworkSubscriber == aSubscriber)
		{
			return true;
		}
	}

	return false;
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

void SharedNetworkManager::AddNetworkMessage(std::vector<char> aBuffer, unsigned int aTargetID)
{
	if (myIsOnline == true)
	{
		mySendBuffer[myCurrentSendBuffer ^ 1].Add({ aBuffer, aTargetID });
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
		case eNetMessageType::CONNECT_REPLY:
			UnpackAndHandle(NetMessageConnectReply(), buffer);
			break;
		case eNetMessageType::ON_CONNECT:
			UnpackAndHandle(NetMessageRequestConnect(), buffer);
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
		case eNetMessageType::START_GAME:
			UnpackAndHandle(NetMessageStartGame(), buffer);
			break;
		case eNetMessageType::LEVEL_LOADED:
			UnpackAndHandle(NetMessageLevelLoaded(), buffer);
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
			DL_ASSERT("Unhandled network message type");
			break;
		}
	}
}

void SharedNetworkManager::ReceiveNetworkMessage(const NetMessagePingReply&, const sockaddr_in&)
{
	myMS = myResponsTime * 1000.f;
}

void SharedNetworkManager::ReceiveNetworkMessage(const NetMessageImportantReply& aMessage, const sockaddr_in&)
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