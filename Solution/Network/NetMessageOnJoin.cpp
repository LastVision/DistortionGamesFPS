#include "stdafx.h"
#include "NetMessageOnJoin.h"
#include "NetworkMessageTypes.h"

NetMessageOnJoin::NetMessageOnJoin(unsigned short anID)
{
	myID = int(eNetMessageType::ON_JOIN);
	mySenderID = anID;
}


NetMessageOnJoin::NetMessageOnJoin()
{
	myID = int(eNetMessageType::ON_JOIN);
}

NetMessageOnJoin::~NetMessageOnJoin()
{
}

void NetMessageOnJoin::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

void NetMessageOnJoin::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

}
