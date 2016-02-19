#include "stdafx.h"
#include "NetMessageOnJoin.h"
#include "NetworkMessageTypes.h"

NetMessageOnJoin::NetMessageOnJoin()
{
}


NetMessageOnJoin::~NetMessageOnJoin()
{
}

void NetMessageOnJoin::Init()
{
	myID = int(eNetMessageType::ON_JOIN);
}

void NetMessageOnJoin::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void NetMessageOnJoin::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

}
