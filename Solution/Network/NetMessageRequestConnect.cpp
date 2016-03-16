#include "stdafx.h"
#include "NetMessageRequestConnect.h"
#include "NetworkMessageTypes.h"
#include <MemoryTracker.h>
NetMessageRequestConnect::NetMessageRequestConnect(const std::string& aName, short aServerID)
{
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
	myName = aName;
	myServerID = aServerID;
}

NetMessageRequestConnect::NetMessageRequestConnect()
{
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
}

NetMessageRequestConnect::NetMessageRequestConnect(sockaddr_in anAddress)
{
	anAddress;
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
}

NetMessageRequestConnect::NetMessageRequestConnect(const std::string& aName, short aServerID, short aOtherClientID)
{
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
	myName = aName;
	myServerID = aServerID;
	myOtherClientID = aOtherClientID;
}

NetMessageRequestConnect::~NetMessageRequestConnect()
{
}

void NetMessageRequestConnect::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myServerID);
	SERIALIZE(aStream, myOtherClientID);
}

void NetMessageRequestConnect::DoDeSerialize(StreamType& aStream)
{

	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myServerID);
	DESERIALIZE(aStream, myOtherClientID);
}
