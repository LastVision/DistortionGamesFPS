#pragma once
#include "NetImportantMessage.h"
class NetMessageDisconnect : public NetImportantMessage
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

