#include "stdafx.h"
#include "NetMessageRequestStartGame.h"
#include "NetworkMessageTypes.h"

NetMessageRequestStartGame::NetMessageRequestStartGame()
{
	myID = int(eNetMessageType::REQUEST_START_GAME);
}