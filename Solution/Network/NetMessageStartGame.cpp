#include "stdafx.h"
#include "NetMessageStartGame.h"

NetMessageStartGame::NetMessageStartGame(int aLevelID)
	: myLevelID(aLevelID)
{
	myID = int(eNetMessageType::START_GAME);
}

NetMessageStartGame::NetMessageStartGame()
	: myLevelID(INT_MAX)
{
	myID = int(eNetMessageType::START_GAME);
}

void NetMessageStartGame::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myLevelID);
}

void NetMessageStartGame::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myLevelID);
}
