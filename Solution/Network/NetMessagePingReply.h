#pragma once
#include "NetMessage.h"
class NetMessagePingReply : public NetMessage
{
public:
	NetMessagePingReply();
	~NetMessagePingReply();
	void Init();
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

