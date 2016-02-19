#pragma once
#include "NetMessage.h"
class CNetMessagePingReply : public CNetMessage
{
public:
	CNetMessagePingReply();
	~CNetMessagePingReply();
	void Init();
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

