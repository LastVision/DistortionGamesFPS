#include "stdafx.h"
#include "NetMessagePingRequest.h"
#include "NetworkMessageTypes.h"

CNetMessagePingRequest::CNetMessagePingRequest()
{
}


CNetMessagePingRequest::~CNetMessagePingRequest()
{
}

void CNetMessagePingRequest::Init()
{
	myID = int(eNetMessageType::PING_REQUEST);
}

void CNetMessagePingRequest::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void CNetMessagePingRequest::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
