#include "stdafx.h"
#include "NetMessageOnJoin.h"
#include "NetworkMessageTypes.h"

NetMessageOnJoin::NetMessageOnJoin(const std::string& aName, unsigned int aSenderID)
	: myName(aName)
{
	myID = int(eNetMessageType::ON_JOIN);
	mySenderID = aSenderID;
}

NetMessageOnJoin::NetMessageOnJoin()
{
	myID = int(eNetMessageType::ON_JOIN);
	myName = "Not Initialized";
}

NetMessageOnJoin::~NetMessageOnJoin()
{
}

void NetMessageOnJoin::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myName);
}

void NetMessageOnJoin::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myName);
}
