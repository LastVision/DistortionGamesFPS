#include "stdafx.h"
#include "NetMessagePosition.h"
#include "NetworkMessageTypes.h"

NetMessagePosition::NetMessagePosition(const CU::Vector3<float>& aPos, float aRotationY, unsigned int aGID)
{
	myPosition = aPos;
	myRotationY = aRotationY;
	myGID = aGID;
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
	SERIALIZE(aStream, myRotationY);
	SERIALIZE(aStream, myGID);
}

void NetMessagePosition::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myRotationY);
	DESERIALIZE(aStream, myGID);
}
