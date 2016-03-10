#pragma once
#include "NetImportantMessage.h"
class NetMessageOnJoin : public NetImportantMessage
{
public:
	NetMessageOnJoin();
	NetMessageOnJoin(unsigned short aSenderID);
	~NetMessageOnJoin();

protected:
	
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

