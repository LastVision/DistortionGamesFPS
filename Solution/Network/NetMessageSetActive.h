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
	: myShouldActivate(aActivate)
	, myGID(aGID)
{
	myID = static_cast<int>(eNetMessageType::SET_ACTIVE);
}

inline NetMessageSetActive::NetMessageSetActive()
{
	myID = static_cast<int>(eNetMessageType::SET_ACTIVE);
}