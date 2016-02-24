#pragma once
#include "NetMessage.h"
class NetMessagePingRequest : public NetMessage
{
public:
	NetMessagePingRequest();
	~NetMessagePingRequest();
	void Init();
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

