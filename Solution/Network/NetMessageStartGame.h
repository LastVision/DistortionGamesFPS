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

inline NetMessageStartGame::NetMessageStartGame(int aLevelID)
	: NetImportantMessage(eNetMessageType::START_GAME)
	, myLevelID(aLevelID)
{
}

inline NetMessageStartGame::NetMessageStartGame()
	: NetImportantMessage(eNetMessageType::START_GAME)
	, myLevelID(INT_MAX)
{
}

inline void NetMessageStartGame::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myLevelID);
}

inline void NetMessageStartGame::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myLevelID);
}
