#pragma once
#include "NetMessage.h"
class NetMessageOnJoin : public NetMessage
{
public:
	NetMessageOnJoin();
	~NetMessageOnJoin();
	void Init();

protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

