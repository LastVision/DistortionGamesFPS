#include "stdafx.h"
#include "NetMessagePingReply.h"
#include "NetworkMessageTypes.h"


CNetMessagePingReply::CNetMessagePingReply()
{
}


CNetMessagePingReply::~CNetMessagePingReply()
{
}

void CNetMessagePingReply::Init()
{
	myID = int(eNetMessageType::PING_REPLY);
}

void CNetMessagePingReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void CNetMessagePingReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
