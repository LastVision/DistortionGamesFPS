#include "stdafx.h"
#include "NetMessageAddEnemy.h"
#include "NetworkMessageTypes.h"

NetMessageAddEnemy::NetMessageAddEnemy()
	: NetImportantMessage(eNetMessageType::ADD_ENEMY)
{
}


NetMessageAddEnemy::NetMessageAddEnemy(const CU::Vector3<float> &aPosition, unsigned int aGID)
	: NetImportantMessage(eNetMessageType::ADD_ENEMY)
	, myPosition(aPosition)
	, myGID(aGID)
{
}

NetMessageAddEnemy::~NetMessageAddEnemy()
{
}

void NetMessageAddEnemy::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myGID);
}

void NetMessageAddEnemy::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myGID);
}
