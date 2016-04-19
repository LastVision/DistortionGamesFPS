#pragma once
#include "NetMessage.h"

class NetMessageActivateSpawnpoint : public NetMessage
{
public:
	NetMessageActivateSpawnpoint(unsigned int aGID);
	NetMessageActivateSpawnpoint();

	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageActivateSpawnpoint::NetMessageActivateSpawnpoint()
	: NetMessage(eNetMessageType::ACTIVATE_SPAWNPOINT)
{

}

inline NetMessageActivateSpawnpoint::NetMessageActivateSpawnpoint(unsigned int aGID)
	: NetMessage(eNetMessageType::ACTIVATE_SPAWNPOINT)
	, myGID(aGID)
{
}

inline void NetMessageActivateSpawnpoint::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageActivateSpawnpoint::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myGID);
}


