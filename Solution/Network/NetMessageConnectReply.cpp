#include "stdafx.h"
#include "NetMessageConnectReply.h"


NetMessageConnectReply::NetMessageConnectReply(eType aType, unsigned int aGID)
	: myType(aType)
	, myGID(aGID)
{
	myID = int(eNetMessageType::CONNECT_REPLY);
}

NetMessageConnectReply::NetMessageConnectReply()
{
	myID = int(eNetMessageType::CONNECT_REPLY);
}


NetMessageConnectReply::~NetMessageConnectReply()
{
}

void NetMessageConnectReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myType);
	SERIALIZE(aStream, myGID);
}

void NetMessageConnectReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myType);
	DESERIALIZE(aStream, myGID);
}

