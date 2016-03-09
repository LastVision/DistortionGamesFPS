#include "stdafx.h"
#include "SharedNetworkManager.h"

#include <NetMessageConnectMessage.h>
#include <NetMessageOnJoin.h>
#include <NetMessageDisconnect.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <NetMessagePosition.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageOnHit.h>

#define BUFFERSIZE 512

void SharedNetworkManager::Initiate()
{
	myReceieveBuffer[0].Init(16384);
	myReceieveBuffer[1].Init(16384);
	mySendBuffer[0].Init(16384);
	mySendBuffer[1].Init(16384);
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
{
}

SharedNetworkManager::~SharedNetworkManager()
{

}

void SharedNetworkManager::Update(float aDelta)
{
	SwapBuffer();
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
		case eNetMessageType::ON_CONNECT:
			UnpackAndHandle(NetMessageConnectMessage(), buffer);
			break;
		case eNetMessageType::ON_JOIN:
			UnpackAndHandle(NetMessageOnJoin(), buffer);
			break;
		case eNetMessageType::ON_DISCONNECT:
			UnpackAndHandle(NetMessageDisconnect(), buffer);
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
			UnpackAndHandle(NetMessageOnHit(), buffer);
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

void SharedNetworkManager::HandleMessage(const NetMessageConnectMessage&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessagePingRequest&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessageDisconnect&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessageOnJoin&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessagePosition&, const sockaddr_in&) {}
void SharedNetworkManager::HandleMessage(const NetMessageAddEnemy&, const sockaddr_in&){}
void SharedNetworkManager::HandleMessage(const NetMessageOnHit&, const sockaddr_in&){}
