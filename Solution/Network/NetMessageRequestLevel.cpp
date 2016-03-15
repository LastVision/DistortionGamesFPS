#include "stdafx.h"
#include "NetMessageRequestLevel.h"
#include "NetworkMessageTypes.h"

NetMessageRequestLevel::NetMessageRequestLevel()
{
	myID = int(eNetMessageType::REQUEST_LEVEL);
}


NetMessageRequestLevel::NetMessageRequestLevel(int aLevelID)
{
	myID = int(eNetMessageType::REQUEST_LEVEL);
	myLevelID = aLevelID;
}

NetMessageRequestLevel::~NetMessageRequestLevel()
{
}

void NetMessageRequestLevel::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myLevelID);
}

void NetMessageRequestLevel::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myLevelID);
}
