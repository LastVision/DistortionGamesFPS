#pragma once
#include "NetImportantMessage.h"
class NetMessageStartGame : public NetImportantMessage
{
public:
	NetMessageStartGame(int aLevelID);
	NetMessageStartGame();

	int myLevelID;

protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

