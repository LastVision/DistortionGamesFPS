#pragma once
#include "NetImportantMessage.h"

class NetMessageLevelComplete : public NetImportantMessage
{
public:
	NetMessageLevelComplete();
};

inline NetMessageLevelComplete::NetMessageLevelComplete()
	: NetImportantMessage(eNetMessageType::LEVEL_COMPLETE)
{
}
