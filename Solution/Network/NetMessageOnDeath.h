#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"
class NetMessageOnDeath : public NetImportantMessage
{
public:
	NetMessageOnDeath();
	NetMessageOnDeath(eNetMessageType aType, const unsigned int aGID);
	~NetMessageOnDeath();

	unsigned int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

