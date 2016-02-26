#include "stdafx.h"
#include "SharedNetworkManager.h"

#include <iostream>
#include <NetMessageConnectMessage.h>
#include <NetMessagePosition.h>
#include <NetMessagePingRequest.h>
#include <NetMessagePingReply.h>
#include <PostMaster.h>
#include <PostNetPositionMessage.h>
#include <sstream>
#include <thread>
#include <Windows.h>

#define BUFFERSIZE 512
#ifdef _DEBUG
#pragma region ThreadNaming
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
void SetThreadName(DWORD dwThreadID, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
#pragma warning(pop)
}

void _SetThreadName(std::thread::id& anID, const char* aThreadName)
{
	std::stringstream ss;
	ss << anID;
	DWORD id;
	ss >> id;
	SetThreadName(id, aThreadName);
}
#pragma endregion
#endif

void SharedNetworkManager::Initiate()
{
	myReceieveBuffer[0].Init(16384);
	myReceieveBuffer[1].Init(16384);
	mySendBuffer[0].Init(16384);
	mySendBuffer[1].Init(16384);
	myPingTime = 0.f;
}

void SharedNetworkManager::StartNetwork()
{

	myIsRunning = true;
	myReceieveThread = new std::thread([&]{ReceieveThread(); });
	mySendThread = new std::thread([&]{SendThread(); });

#ifdef _DEBUG
	if (myIsServer == true)
	{
		_SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Server");
		_SetThreadName(mySendThread->get_id(), "Send Thread - Server");
	}
	else
	{
		_SetThreadName(myReceieveThread->get_id(), "Receieve Thread - Client");
		_SetThreadName(mySendThread->get_id(), "Send Thread - Client");
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
	SwapBuffers();
	myPingTime -= aDelta;
	myResponsTime += aDelta;
	if (myPingTime <= 0.f)
	{
		AddMessage(NetMessagePingRequest());
		myPingTime = 1.f;
	}
	HandleMessage();
}


void SharedNetworkManager::SwapBuffers()
{
	//mySendBuffer[myCurrentSendBuffer].RemoveAll();
	myReceieveBuffer[myCurrentBuffer].RemoveAll();
	myCurrentBuffer ^= 1;
}

void SharedNetworkManager::HandleMessage()
{
	for (Buffer& buf : myReceieveBuffer[myCurrentBuffer])
	{
		eNetMessageType type = ReadType(buf.myData);
		switch (type)
		{
		case eNetMessageType::NONE:
			break;
		case eNetMessageType::ON_CONNECT:
		{
			NetMessageConnectMessage connect;
			connect.UnPackMessage(buf.myData, buf.myLength);
			HandleMessage(connect, buf.mySenderAddress);
			break;
		}
		case eNetMessageType::ON_JOIN:
			break;
		case eNetMessageType::PING_REQUEST:
		{
			NetMessagePingRequest request;
			request.UnPackMessage(buf.myData, buf.myLength);
			HandleMessage(request, buf.mySenderAddress);
			break;
		}
		case eNetMessageType::PING_REPLY:
		{
			NetMessagePingReply reply;
			reply.UnPackMessage(buf.myData, buf.myLength);
			HandleMessage(reply, buf.mySenderAddress);
			break;
		}
		case eNetMessageType::POSITION:
		{
			NetMessagePosition pos;
			pos.UnPackMessage(buf.myData, buf.myLength);
			PostMaster::GetInstance()->SendMessage(PostNetPositionMessage(pos.myPosition));
			break;
		}
		default:
			break;
		}
	}
}

void SharedNetworkManager::HandleMessage(const NetMessageConnectMessage&, const sockaddr_in&){}

void SharedNetworkManager::HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress)
{
	AddMessage(NetMessagePingReply());
}

void SharedNetworkManager::HandleMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress)
{
	myMS = myResponsTime * 1000.f;
	myResponsTime = 0.f;
}

eNetMessageType SharedNetworkManager::ReadType(const char* aBuffer)
{
	return static_cast<eNetMessageType>(aBuffer[0]);
}

eNetMessageType SharedNetworkManager::ReadType(const std::vector<char>& aBuffer)
{
	return static_cast<eNetMessageType>(aBuffer[0]);
}

float SharedNetworkManager::GetResponsTime() const
{
	return myMS;
}

void SharedNetworkManager::AddNetworkMessage(std::vector<char> aBuffer)
{
	if (myIsOnline == true)
	{
		/*if (myIsServer == true && ReadType(aBuffer) == eNetMessageType::PING_REPLY)
		{
			std::cout << "Message added to send buffer!\n";
		}*/
		mySendBuffer[myCurrentSendBuffer ^ 1].Add(aBuffer);
	}
}
