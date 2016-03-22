#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"
class NetMessageHealthPack : public NetImportantMessage
{
public:
	NetMessageHealthPack();
	NetMessageHealthPack(int aHealAmount);
	~NetMessageHealthPack();

	int myHealAmount;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageHealthPack::NetMessageHealthPack()
	: NetImportantMessage(eNetMessageType::HEALTH_PACK)
{
}

inline NetMessageHealthPack::NetMessageHealthPack(int aHealAmount)
	: NetImportantMessage(eNetMessageType::HEALTH_PACK)
	, myHealAmount(aHealAmount)
{
}

inline NetMessageHealthPack::~NetMessageHealthPack()
{
}

inline void NetMessageHealthPack::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myHealAmount);
}

inline void NetMessageHealthPack::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myHealAmount);
}