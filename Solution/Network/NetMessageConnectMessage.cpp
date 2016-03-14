#include "stdafx.h"
#include "NetMessageConnectMessage.h"
#include "NetworkMessageTypes.h"
#include <MemoryTracker.h>
NetMessageConnectMessage::NetMessageConnectMessage(const std::string& aName, short aServerID)
{
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
	myName = aName;
	myServerID = aServerID;
}

NetMessageConnectMessage::NetMessageConnectMessage()
{
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
}

NetMessageConnectMessage::NetMessageConnectMessage(sockaddr_in anAddress)
{
	anAddress;
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
}

NetMessageConnectMessage::NetMessageConnectMessage(const std::string& aName, short aServerID, short aOtherClientID)
{
	myID = static_cast<uint8_t>(eNetMessageType::ON_CONNECT);
	myName = aName;
	myServerID = aServerID;
	myOtherClientID = aOtherClientID;
}

NetMessageConnectMessage::~NetMessageConnectMessage()
{
}

void NetMessageConnectMessage::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myServerID);
	SERIALIZE(aStream, myOtherClientID);
}

void NetMessageConnectMessage::DoDeSerialize(StreamType& aStream)
{

	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myServerID);
	DESERIALIZE(aStream, myOtherClientID);
}
