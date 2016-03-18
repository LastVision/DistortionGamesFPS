#pragma once
#include "NetImportantMessage.h"

class NetMessageLevelLoaded : public NetImportantMessage
{
public:
	NetMessageLevelLoaded();
};

inline NetMessageLevelLoaded::NetMessageLevelLoaded()
	: NetImportantMessage(eNetMessageType::LEVEL_LOADED)
{
}
