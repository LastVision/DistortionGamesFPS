#pragma once
#include "NetImportantMessage.h"
class NetMessageDisconnect : public NetImportantMessage
{
public:
	NetMessageDisconnect(unsigned int aClientID);
	NetMessageDisconnect();
	~NetMessageDisconnect();

	unsigned int myClientID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

