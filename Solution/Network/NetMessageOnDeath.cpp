#include "stdafx.h"
#include "NetMessageOnDeath.h"

NetMessageOnDeath::NetMessageOnDeath(eNetMessageType aType, const unsigned int aNetworkID)
{
	myID = static_cast<uint8_t>(aType);
	myNetworkID = aNetworkID;
}

NetMessageOnDeath::NetMessageOnDeath()
{
	myID = static_cast<uint8_t>(eNetMessageType::ENEMY_ON_DEATH);
}

NetMessageOnDeath::~NetMessageOnDeath()
{
}

void NetMessageOnDeath::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myNetworkID);
}

void NetMessageOnDeath::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myNetworkID);
}
