#pragma once
#include "NetMessage.h"
class CNetMessagePingRequest : public CNetMessage
{
public:
	CNetMessagePingRequest();
	~CNetMessagePingRequest();
	void Init();
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

