#include "stdafx.h"
#include "NetMessagePosition.h"
#include "NetworkMessageTypes.h"

NetMessagePosition::NetMessagePosition(const CU::Vector3<float>& aPos, unsigned int aNetworkID)
{
	myPosition = aPos;
	myNetworkID = aNetworkID;
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
	SERIALIZE(aStream, myNetworkID);
}

void NetMessagePosition::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myNetworkID);
}
