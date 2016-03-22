#pragma once
#include "NetImportantMessage.h"
class NetMessageRequestStartLevel : public NetImportantMessage
{
public:
	NetMessageRequestStartLevel();
};

inline NetMessageRequestStartLevel::NetMessageRequestStartLevel()
	: NetImportantMessage(eNetMessageType::REQUEST_START_LEVEL)
{
}