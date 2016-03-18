#pragma once
#include "NetMessage.h"
class NetMessagePingReply : public NetMessage
{
public:
	NetMessagePingReply();
	~NetMessagePingReply();
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessagePingReply::NetMessagePingReply()
	: NetMessage(eNetMessageType::PING_REPLY)
{
}

inline NetMessagePingReply::~NetMessagePingReply()
{
}

inline void NetMessagePingReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

inline void NetMessagePingReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
