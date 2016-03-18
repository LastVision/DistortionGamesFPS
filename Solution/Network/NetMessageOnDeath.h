#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"
class NetMessageOnDeath : public NetImportantMessage
{
public:
	NetMessageOnDeath();
	NetMessageOnDeath(eNetMessageType aType, const unsigned int aGID);
	~NetMessageOnDeath();

	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageOnDeath::NetMessageOnDeath(eNetMessageType aType, const unsigned int aGID)
	: NetImportantMessage(aType)
	, myGID(aGID)
{
}

inline NetMessageOnDeath::NetMessageOnDeath()
	: NetImportantMessage(eNetMessageType::ENEMY_ON_DEATH)
{
}

inline NetMessageOnDeath::~NetMessageOnDeath()
{
}

inline void NetMessageOnDeath::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageOnDeath::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myGID);
}
