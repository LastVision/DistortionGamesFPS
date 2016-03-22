#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"
class NetMessageOnDeath : public NetImportantMessage
{
public:
	NetMessageOnDeath();
	NetMessageOnDeath(const unsigned int aGID);
	~NetMessageOnDeath();

	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageOnDeath::NetMessageOnDeath(const unsigned int aGID)
	: NetImportantMessage(eNetMessageType::ON_DEATH)
	, myGID(aGID)
{
}

inline NetMessageOnDeath::NetMessageOnDeath()
	: NetImportantMessage(eNetMessageType::ON_DEATH)
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
