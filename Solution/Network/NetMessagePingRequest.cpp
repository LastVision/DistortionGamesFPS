#include "stdafx.h"
#include "NetMessagePingRequest.h"
#include "NetworkMessageTypes.h"

NetMessagePingRequest::NetMessagePingRequest()
{
}


NetMessagePingRequest::~NetMessagePingRequest()
{
}

void NetMessagePingRequest::Init()
{
	myID = int(eNetMessageType::PING_REQUEST);
}

void NetMessagePingRequest::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void NetMessagePingRequest::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
