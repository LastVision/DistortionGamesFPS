#pragma once
#include "NetImportantMessage.h"
class NetMessageRequestStartGame : public NetImportantMessage
{
public:
	NetMessageRequestStartGame();
};

inline NetMessageRequestStartGame::NetMessageRequestStartGame()
	: NetImportantMessage(eNetMessageType::REQUEST_START_GAME)
{
}