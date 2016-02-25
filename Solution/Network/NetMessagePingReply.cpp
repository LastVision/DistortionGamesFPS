#include "stdafx.h"
#include "NetMessagePingReply.h"
#include "NetworkMessageTypes.h"


NetMessagePingReply::NetMessagePingReply()
{
	myID = int(eNetMessageType::PING_REPLY);
}


NetMessagePingReply::~NetMessagePingReply()
{
}

void NetMessagePingReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void NetMessagePingReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
