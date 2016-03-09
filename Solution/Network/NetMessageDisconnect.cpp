#include "stdafx.h"
#include "NetMessageDisconnect.h"
#include "NetworkMessageTypes.h"

NetMessageDisconnect::NetMessageDisconnect(const unsigned short aClientID)
{
	myID = static_cast<int>(eNetMessageType::ON_DISCONNECT);
	myClientID = aClientID;
}

NetMessageDisconnect::NetMessageDisconnect()
{
	myID = static_cast<int>(eNetMessageType::ON_DISCONNECT);
}

NetMessageDisconnect::~NetMessageDisconnect()
{
}

void NetMessageDisconnect::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myClientID);
}

void NetMessageDisconnect::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myClientID);
}
