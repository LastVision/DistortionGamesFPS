#include "stdafx.h"
#include "NetMessagePingReply.h"
#include "NetworkMessageTypes.h"


NetMessagePingReply::NetMessagePingReply()
{
}


NetMessagePingReply::~NetMessagePingReply()
{
}

void NetMessagePingReply::Init()
{
	myID = int(eNetMessageType::PING_REPLY);
}

void NetMessagePingReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void NetMessagePingReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
