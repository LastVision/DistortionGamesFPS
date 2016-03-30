#pragma once
#include "NetImportantMessage.h"

class NetMessageRequestServer : public NetImportantMessage
{
public:
	NetMessageRequestServer();
};

inline NetMessageRequestServer::NetMessageRequestServer()
	: NetImportantMessage(eNetMessageType::SERVER_REQUEST)
{
}