#include "stdafx.h"
#include "SharedNetworkManager.h"

#include "NetMessage.h"
#include "NetMessageActivateSpawnpoint.h"
#include "NetMessageActivateUnit.h"
#include "NetMessageAllClientsComplete.h"
#include "NetMessageImportantReply.h"
#include "NetMessageConnectReply.h"
#include "NetMessageDisplayMarker.h"
#include "NetMessageDisplayRespawn.h"
#include "NetMessageEntityState.h"
#include "NetMessageRequestConnect.h"
#include "NetMessageOnJoin.h"
#include "NetMessageHealth.h"
#include "NetMessageHealthPack.h"
#include "NetMessageDisconnect.h"
#include "NetMessageSetLevel.h"
#include "NetMessageRayCastRequest.h"
#include "NetMessageRequestStartLevel.h"
#include "NetMessagePingRequest.h"
#include "NetMessagePingReply.h"
#include "NetMessagePosition.h"
#include "NetMessagePressE.h"
#include "NetMessageReplyServer.h"
#include "NetMessageRequestServer.h"
#include "NetMessageOnHit.h"
#include "NetMessageOnDeath.h"
#include "NetMessageExplosion.h"
#include "NetMessageShootGrenade.h"
#include "NetMessageSetActive.h"
#include "NetMessageLoadLevel.h"
#include "NetMessageLevelLoaded.h"
#include "NetMessageEnemyShooting.h"
#include "NetMessageLevelComplete.h"
#include "NetMessageText.h"

#include <IPHlpApi.h>
#pragma comment(lib, "IPHLPAPI.lib")

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
	myImportantReceivedMessages.Init(16384);
	myImportantID = 0;
	myPingTime = 0.f;
	myReceieveIsDone = true;
	myMainIsDone = true;
	for (int i = 0; i < static_cast<int>(eNetMessageType::_COUNT); ++i)
	{
		mySubscribers[i].Init(64);
	}
	Subscribe(eNetMessageType::IMPORTANT_REPLY, this);
	Subscribe(eNetMessageType::SERVER_REPLY, this);
	Subscribe(eNetMessageType::SERVER_REQUEST, this);
	myHasSent = false;
	myAllowSendWithoutSubscribers = false;
	myStopSendMessages = false;

	myMessageGameIdentifier = 1496;
}

void SharedNetworkManager::StartNetwork(unsigned int aPortNum)
{

	myIsRunning = true;
	myReceieveThread = new std::thread([&] {ReceieveThread(); });
	mySendThread = new std::thread([&] {SendThread(); });
	myPingThread = new std::thread([&] {PingThread(); });

#ifdef _DEBUG
	if (myIsServer == true)
	{
		CU::SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Server");
		CU::SetThreadName(mySendThread->get_id(), "Send Thread - Server");
		CU::SetThreadName(myPingThread->get_id(), "Ping Thread - Server");
	}
	else
	{
		CU::SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Client");
		CU::SetThreadName(mySendThread->get_id(), "Send Thread - Client");
		CU::SetThreadName(myPingThread->get_id(), "Ping Thread - Client");
	}
#endif
	SetupBroadcastAddress(aPortNum);
}

void SharedNetworkManager::SetupBroadcastAddress(unsigned int aPortNumber)
{
	ZeroMemory(&myBroadcastAddress, sizeof(myBroadcastAddress));
	hostent* localHosy = gethostbyname("");
	char* localIP = inet_ntoa(*(struct in_addr*)*localHosy->h_addr_list);

	IP_ADAPTER_INFO * FixedInfo;
	ULONG ulOutBufLen;

	FixedInfo = (IP_ADAPTER_INFO *)GlobalAlloc(GPTR, sizeof(IP_ADAPTER_INFO));
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (ERROR_SUCCESS != GetAdaptersInfo(FixedInfo, &ulOutBufLen))
	{
		DL_ASSERT("Cound not get the adapter information!");
		return;
	}

	char* subnetmask = FixedInfo->IpAddressList.IpMask.String;
	in_addr hostIP, mask;

	if (inet_pton(AF_INET, localIP, &hostIP) == 1 &&
		inet_pton(AF_INET, subnetmask, &mask) == 1)
	{
		myBroadcastAddress.sin_addr.S_un.S_addr = hostIP.S_un.S_addr | ~mask.S_un.S_addr;
	}

	myBroadcastAddress.sin_port = htons(static_cast<uint16_t>(aPortNumber));
	myBroadcastAddress.sin_family = AF_INET;
	


}

SharedNetworkManager::SharedNetworkManager()
	: myIsOnline(false)
	, myIsRunning(false)
	, myCurrentBuffer(0)
	, myCurrentSendBuffer(0)
{
}

SharedNetworkManager::~SharedNetworkManager()
{
	UnSubscribe(eNetMessageType::IMPORTANT_REPLY, this);
	UnSubscribe(eNetMessageType::SERVER_REPLY, this);
	UnSubscribe(eNetMessageType::SERVER_REQUEST, this);

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
		myDataToPrint = myDataSent;
		myDataSent = 0;
		myPingTime = 1.f;
		myRepliesPerSecond = 1.f / myReplyCount;
		myReplyCount = 0;
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
	return static_cast<eNetMessageType>(aBuffer[2]);
}

eNetMessageType SharedNetworkManager::ReadType(const std::vector<char>& aBuffer)
{
	return static_cast<eNetMessageType>(aBuffer[2]);
}

unsigned long long SharedNetworkManager::GetResponsTime() const
{
	return myMS;
}

double SharedNetworkManager::GetDataSent() const
{
	return myDataToPrint / 1024;
}

void SharedNetworkManager::AllowSendWithoutSubscriber(bool aAllow)
{
	myAllowSendWithoutSubscribers = aAllow;
}

void SharedNetworkManager::AddNetworkMessage(std::vector<char> aBuffer, unsigned int aTargetID)
{
	if (myIsOnline == true)
	{
		mySendBuffer[myCurrentSendBuffer ^ 1].Add({ aBuffer, aTargetID });
	}
}

void SharedNetworkManager::AddNetworkMessage(std::vector<char> aBuffer, sockaddr_in aTargetAddress)
{
	mySendBuffer[myCurrentSendBuffer ^ 1].Add({ aBuffer, aTargetAddress });
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
		case eNetMessageType::SET_LEVEL:
			UnpackAndHandle(NetMessageSetLevel(), buffer);
			break;
		case eNetMessageType::REQUEST_START_LEVEL:
			UnpackAndHandle(NetMessageRequestStartLevel(), buffer);
			break;
		case eNetMessageType::LOAD_LEVEL:
			UnpackAndHandle(NetMessageLoadLevel(), buffer);
			break;
		case eNetMessageType::LEVEL_LOADED:
			UnpackAndHandle(NetMessageLevelLoaded(), buffer);
			break;
		case eNetMessageType::LEVEL_COMPLETE:
			UnpackAndHandle(NetMessageLevelComplete(), buffer);
			break;
		case eNetMessageType::ALL_CLIENTS_COMPLETE:
			UnpackAndHandle(NetMessageAllClientsComplete(), buffer);
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
		case eNetMessageType::ON_HIT:
			UnpackAndHandle(NetMessageOnHit(), buffer);
			break;
		case eNetMessageType::ON_DEATH:
			UnpackAndHandle(NetMessageOnDeath(), buffer);
			break;
		case eNetMessageType::SET_ACTIVE:
			UnpackAndHandle(NetMessageSetActive(), buffer);
			break;
		case eNetMessageType::ENTITY_STATE:
			UnpackAndHandle(NetMessageEntityState(), buffer);
			break;
		case eNetMessageType::ENEMY_SHOOTING:
			UnpackAndHandle(NetMessageEnemyShooting(), buffer);
			break;
		case eNetMessageType::HEALTH:
			UnpackAndHandle(NetMessageHealth(), buffer);
			break;
		case eNetMessageType::HEALTH_PACK:
			UnpackAndHandle(NetMessageHealthPack(), buffer);
			break;
		case eNetMessageType::SHOOT_GRENADE:
			UnpackAndHandle(NetMessageShootGrenade(), buffer);
			break;
		case eNetMessageType::TEXT:
			UnpackAndHandle(NetMessageText(), buffer);
			break;
		case eNetMessageType::EXPLOSION:
			UnpackAndHandle(NetMessageExplosion(), buffer);
			break;
		case eNetMessageType::ACTIVATE_SPAWNPOINT:
			UnpackAndHandle(NetMessageActivateSpawnpoint(), buffer);
			break;
		case eNetMessageType::ACTIVATE_UNIT:
			UnpackAndHandle(NetMessageActivateUnit(), buffer);
			break;
		case eNetMessageType::DISPLAY_MARKER:
			UnpackAndHandle(NetMessageDisplayMarker(), buffer);
			break;
		case eNetMessageType::DISPLAY_RESPAWN:
			UnpackAndHandle(NetMessageDisplayRespawn(), buffer);
			break;
		case eNetMessageType::SERVER_REPLY:
			UnpackAndHandle(NetMessageReplyServer(), buffer);
			break;
		case eNetMessageType::SERVER_REQUEST:
			UnpackAndHandle(NetMessageRequestServer(), buffer);
			break;
		case eNetMessageType::RAY_CAST_REQUEST:
			UnpackAndHandle(NetMessageRayCastRequest(), buffer);
			break;
		case eNetMessageType::PRESS_E:
			UnpackAndHandle(NetMessagePressE(), buffer);
			break;
		default:
			DL_ASSERT("Unhandled network message type");
			break;
		}
	}
}

void SharedNetworkManager::ReceiveNetworkMessage(const NetMessageImportantReply& aMessage, const sockaddr_in& aSenderAddress)
{
	for (ImportantMessage& msg : myImportantMessagesBuffer)
	{
		if (aMessage.myImportantID == msg.myImportantID)
		{
			for (ImportantClient& client : msg.mySenders)
			{
				if (client.myGID == UINT_MAX)
				{
					if (client.myNetworkAddress.sin_addr.S_un.S_addr == aSenderAddress.sin_addr.S_un.S_addr)
					{
						client.myHasReplied = true;
						return;
					}
				}
				else if (aMessage.mySenderID == client.myGID)
				{
					client.myHasReplied = true;
					return;
				}
			}
		}
	}
}

float SharedNetworkManager::GetRepliesPerSecond()
{
	return myRepliesPerSecond;
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