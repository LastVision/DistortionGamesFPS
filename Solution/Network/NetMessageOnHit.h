#pragma once
#include "NetMessage.h"
#include "NetworkMessageTypes.h"
class NetMessageOnHit :
	public NetMessage
{
public:
	NetMessageOnHit();
	NetMessageOnHit(float aDamage, unsigned int aGID);
	NetMessageOnHit(eNetMessageType aType, float aDamage, unsigned int aGID);
	~NetMessageOnHit();

	float myDamage;
	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageOnHit::NetMessageOnHit(eNetMessageType aType, float aDamage, unsigned int aGID)
	: NetMessage(aType)
	, myDamage(aDamage)
	, myGID(aGID)
{
}

inline NetMessageOnHit::NetMessageOnHit(float aDamage, unsigned int aGID)
	: NetMessage(eNetMessageType::ENEMY_ON_HIT)
	, myDamage(aDamage)
	, myGID(aGID)
{
}

inline NetMessageOnHit::NetMessageOnHit()
	: NetMessage(eNetMessageType::ENEMY_ON_HIT)
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