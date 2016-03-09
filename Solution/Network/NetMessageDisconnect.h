#pragma once
#include "NetMessage.h"
class NetMessageDisconnect : public NetMessage
{
public:
	NetMessageDisconnect(const unsigned short aClientID);
	NetMessageDisconnect();
	~NetMessageDisconnect();

	unsigned short myClientID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

