#include "stdafx.h"
#include "NetMessageConnectMessage.h"
#include "NetworkMessageTypes.h"

NetMessageConnectMessage::NetMessageConnectMessage()
{
}


NetMessageConnectMessage::~NetMessageConnectMessage()
{
}


void NetMessageConnectMessage::Init(const std::string& aName, short aServerID)
{
	myID = int(eNetMessageType::ON_CONNECT);
	myName = aName;
	myServerID = aServerID;
}

void NetMessageConnectMessage::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myServerID);
}

void NetMessageConnectMessage::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myServerID);
}
