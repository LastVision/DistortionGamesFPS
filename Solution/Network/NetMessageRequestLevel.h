#pragma once
#include "NetImportantMessage.h"

class NetMessageRequestLevel : public NetImportantMessage
{
public:
	NetMessageRequestLevel();
};

inline NetMessageRequestLevel::NetMessageRequestLevel()
	: NetImportantMessage(eNetMessageType::REQUEST_LEVEL)
{
}