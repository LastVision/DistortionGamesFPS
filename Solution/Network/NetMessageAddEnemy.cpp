#include "stdafx.h"
#include "NetMessageAddEnemy.h"
#include "NetworkMessageTypes.h"

NetMessageAddEnemy::NetMessageAddEnemy()
{
	myID = int(eNetMessageType::ADD_ENEMY);
}


NetMessageAddEnemy::NetMessageAddEnemy(const CU::Vector3<float> &aPosition, unsigned int aNetworkID)
{
	myPosition = aPosition;
	myID = int(eNetMessageType::ADD_ENEMY);
	myNetworkID = aNetworkID;
}

NetMessageAddEnemy::~NetMessageAddEnemy()
{
}

void NetMessageAddEnemy::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myNetworkID);
}

void NetMessageAddEnemy::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myNetworkID);
}
