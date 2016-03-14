#pragma once
#include "NetMessage.h"
#include "NetworkMessageTypes.h"
class NetMessageOnHit :
	public NetMessage
{
public:
	NetMessageOnHit();
	NetMessageOnHit(eNetMessageType aType, float aDamage, unsigned int aGID);
	~NetMessageOnHit();

	float myDamage;
	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

