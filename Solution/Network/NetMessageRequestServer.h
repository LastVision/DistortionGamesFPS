#pragma once
#include "NetMessage.h"

class NetMessageRequestServer : public NetMessage
{
public:
	NetMessageRequestServer();
};

inline NetMessageRequestServer::NetMessageRequestServer()
	: NetMessage(eNetMessageType::SERVER_REQUEST)
{
}