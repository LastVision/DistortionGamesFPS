#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"

class NetMessageHealth : public NetImportantMessage
{
public:
	NetMessageHealth();
	NetMessageHealth(int aMaxHealth, int aCurrentHealth, int aGID);
	~NetMessageHealth();

	int myMaxHealth;
	int myCurrentHealth;
	int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageHealth::NetMessageHealth()
	: NetImportantMessage(eNetMessageType::HEALTH)
{
}

inline NetMessageHealth::NetMessageHealth(int aMaxHealth, int aCurrentHealth, int aGID)
	: NetImportantMessage(eNetMessageType::HEALTH)
	, myMaxHealth(aMaxHealth)
	, myCurrentHealth(aCurrentHealth)
	, myGID(aGID)
{
}

inline NetMessageHealth::~NetMessageHealth()
{
}

inline void NetMessageHealth::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myMaxHealth);
	SERIALIZE(aStream, myCurrentHealth);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageHealth::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myMaxHealth);
	DESERIALIZE(aStream, myCurrentHealth);
	DESERIALIZE(aStream, myGID);
}