#pragma once
#include "NetImportantMessage.h"

class NetMessageRequestLevel : public NetImportantMessage
{
public:
	NetMessageRequestLevel(int aLevelID);
	NetMessageRequestLevel();
	~NetMessageRequestLevel();

	int myLevelID;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

