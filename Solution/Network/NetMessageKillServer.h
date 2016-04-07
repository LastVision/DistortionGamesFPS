#pragma once
#include "NetImportantMessage.h"

class NetMessageKillServer : public NetImportantMessage
{
public:
	NetMessageKillServer();
};

inline NetMessageKillServer::NetMessageKillServer()
	: NetImportantMessage(eNetMessageType::KILL_SERVER)
{
}