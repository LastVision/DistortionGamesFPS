#include "stdafx.h"
#include "NetMessagePosition.h"
#include "NetworkMessageTypes.h"

NetMessagePosition::NetMessagePosition()
{
}


NetMessagePosition::~NetMessagePosition()
{
}

void NetMessagePosition::Init()
{
	myID = int(eNetMessageType::POSITION);
}

void NetMessagePosition::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
}

void NetMessagePosition::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
}
