#include "stdafx.h"
#include "NetMessageOnHit.h"

NetMessageOnHit::NetMessageOnHit(eNetMessageType aType, float aDamage, unsigned int aGID)
{
	myDamage = aDamage;
	myGID = aGID;
	myID = uint8_t(aType);
}

NetMessageOnHit::NetMessageOnHit()
{
	myID = int(eNetMessageType::ENEMY_ON_HIT);
}

NetMessageOnHit::~NetMessageOnHit()
{
}

void NetMessageOnHit::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myDamage);
	SERIALIZE(aStream, myGID);
}

void NetMessageOnHit::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myDamage);
	DESERIALIZE(aStream, myGID);
}
