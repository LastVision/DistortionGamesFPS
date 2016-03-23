#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"

class NetMessageExplosion : public NetImportantMessage
{
public:
	NetMessageExplosion();
	NetMessageExplosion(const CU::Vector3<float>& aPosition, unsigned int aGID);
	~NetMessageExplosion();

	CU::Vector3<float> myPosition;
	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageExplosion::NetMessageExplosion()
	: NetImportantMessage(eNetMessageType::EXPLOSION)
{
}

inline NetMessageExplosion::NetMessageExplosion(const CU::Vector3<float>& aPosition, unsigned int aGID)
	: NetImportantMessage(eNetMessageType::EXPLOSION)
	, myPosition(aPosition)
	, myGID(aGID)
{
}

inline NetMessageExplosion::~NetMessageExplosion()
{
}

inline void NetMessageExplosion::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageExplosion::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myGID);
}