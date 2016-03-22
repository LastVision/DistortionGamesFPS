#pragma once
#include "NetMessage.h"
#include "NetworkMessageTypes.h"
class NetMessageOnHit :
	public NetMessage
{
public:
	NetMessageOnHit();
	NetMessageOnHit(int aDamage, unsigned int aGID);
	~NetMessageOnHit();

	int myDamage;
	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageOnHit::NetMessageOnHit()
	: NetMessage(eNetMessageType::ON_HIT)
{
}

inline NetMessageOnHit::NetMessageOnHit(int aDamage, unsigned int aGID)
	: NetMessage(eNetMessageType::ON_HIT)
	, myDamage(aDamage)
	, myGID(aGID)
{
}

inline NetMessageOnHit::~NetMessageOnHit()
{
}

inline void NetMessageOnHit::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myDamage);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageOnHit::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myDamage);
	DESERIALIZE(aStream, myGID);
}