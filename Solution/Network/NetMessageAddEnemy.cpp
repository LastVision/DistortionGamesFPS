#include "stdafx.h"
#include "NetMessageAddEnemy.h"
#include "NetworkMessageTypes.h"

NetMessageAddEnemy::NetMessageAddEnemy()
{
	myID = int(eNetMessageType::ADD_ENEMY);
}


NetMessageAddEnemy::NetMessageAddEnemy(const CU::Vector3<float> &aPosition)
{
	myPosition = aPosition;
	myID = int(eNetMessageType::ADD_ENEMY);
}

NetMessageAddEnemy::~NetMessageAddEnemy()
{
}

void NetMessageAddEnemy::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
}

void NetMessageAddEnemy::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
}
