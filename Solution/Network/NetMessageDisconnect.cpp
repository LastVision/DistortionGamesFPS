#include "stdafx.h"
#include "NetMessageDisconnect.h"
#include "NetworkMessageTypes.h"

NetMessageDisconnect::NetMessageDisconnect(unsigned int aClientID)
	: NetImportantMessage(eNetMessageType::ON_DISCONNECT)
	, myClientID(aClientID)
{
}

NetMessageDisconnect::NetMessageDisconnect()
	: NetImportantMessage(eNetMessageType::ON_DISCONNECT)
{
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
