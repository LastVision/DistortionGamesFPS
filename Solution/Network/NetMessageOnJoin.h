#pragma once
#include "NetMessage.h"
class NetMessageOnJoin : public NetMessage
{
public:
	NetMessageOnJoin();
	NetMessageOnJoin(unsigned short aSenderID);
	~NetMessageOnJoin();

protected:
	
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

