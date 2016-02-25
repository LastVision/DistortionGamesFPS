#include "stdafx.h"
#include "NetMessagePingRequest.h"
#include "NetworkMessageTypes.h"

NetMessagePingRequest::NetMessagePingRequest()
{
	myID = int(eNetMessageType::PING_REQUEST);
}


NetMessagePingRequest::~NetMessagePingRequest()
{
}

void NetMessagePingRequest::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void NetMessagePingRequest::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
