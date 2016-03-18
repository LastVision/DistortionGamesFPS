#pragma once
#include "NetImportantMessage.h"

class NetMessageSetActive : public NetImportantMessage
{
public:
	NetMessageSetActive(bool aActivate, unsigned int aGID);
	NetMessageSetActive();
	~NetMessageSetActive(){};

	bool myShouldActivate;
	unsigned int myGID;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageSetActive::NetMessageSetActive(bool aActivate, unsigned int aGID)
	: NetImportantMessage(eNetMessageType::SET_ACTIVE)
	, myShouldActivate(aActivate)
	, myGID(aGID)
{
}

inline NetMessageSetActive::NetMessageSetActive()
	: NetImportantMessage(eNetMessageType::SET_ACTIVE)
{
}


inline void NetMessageSetActive::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myShouldActivate);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageSetActive::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myShouldActivate);
	DESERIALIZE(aStream, myGID);
}

