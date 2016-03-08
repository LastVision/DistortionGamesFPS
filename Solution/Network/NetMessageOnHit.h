#pragma once
#include "NetMessage.h"
class NetMessageOnHit :
	public NetMessage
{
public:
	NetMessageOnHit();
	NetMessageOnHit(eNetMessageType aType, float aDamage, unsigned int aNetworkID);
	~NetMessageOnHit();

	float myDamage;
	unsigned int myNetworkID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

