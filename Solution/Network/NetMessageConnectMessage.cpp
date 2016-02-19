#include "stdafx.h"
#include "NetMessageConnectMessage.h"
#include "NetworkMessageTypes.h"

CNetMessageConnectMessage::CNetMessageConnectMessage()
{
}


CNetMessageConnectMessage::~CNetMessageConnectMessage()
{
}


void CNetMessageConnectMessage::Init(const std::string& aName, short aServerID)
{
	myID = int(eNetMessageType::ON_CONNECT);
	myName = aName;
	myServerID = aServerID;
}

void CNetMessageConnectMessage::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myServerID);
}

void CNetMessageConnectMessage::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myServerID);
}
