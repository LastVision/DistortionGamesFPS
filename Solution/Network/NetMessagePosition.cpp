#include "stdafx.h"
#include "NetMessagePosition.h"
#include "NetworkMessageTypes.h"

NetMessagePosition::NetMessagePosition(const CU::Vector3<float>& aPos)
{
	myPosition = aPos;
	myID = int(eNetMessageType::POSITION);
}

NetMessagePosition::NetMessagePosition()
{
	myID = int(eNetMessageType::POSITION);
}

NetMessagePosition::~NetMessagePosition()
{
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
