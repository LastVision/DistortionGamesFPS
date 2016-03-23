#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"

class NetMessageShootGrenade : public NetImportantMessage
{
public:
	NetMessageShootGrenade();
	NetMessageShootGrenade(int aForceStrength);
	~NetMessageShootGrenade();

	int myForceStrength;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageShootGrenade::NetMessageShootGrenade()
	: NetImportantMessage(eNetMessageType::SHOOT_GRENADE)
{
}

inline NetMessageShootGrenade::NetMessageShootGrenade(int aForceStrength)
	: NetImportantMessage(eNetMessageType::SHOOT_GRENADE)
	, myForceStrength(aForceStrength)
{
}

inline NetMessageShootGrenade::~NetMessageShootGrenade()
{
}

inline void NetMessageShootGrenade::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myForceStrength);
}

inline void NetMessageShootGrenade::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myForceStrength);
}