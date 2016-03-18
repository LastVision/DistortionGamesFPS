#pragma once
#include "NetMessage.h"
class NetMessagePingRequest : public NetMessage
{
public:
	NetMessagePingRequest();
	~NetMessagePingRequest();
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessagePingRequest::NetMessagePingRequest()
	: NetMessage(eNetMessageType::PING_REQUEST)
{
}

inline NetMessagePingRequest::~NetMessagePingRequest()
{
}

inline void NetMessagePingRequest::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
}

inline void NetMessagePingRequest::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
