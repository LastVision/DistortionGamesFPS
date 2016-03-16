#pragma once
#include "Message.h"
struct PostMasterNetLevelLoadedMessage : public Message
{
	PostMasterNetLevelLoadedMessage(unsigned int aSender);

	const unsigned int mySender;
};

inline PostMasterNetLevelLoadedMessage::PostMasterNetLevelLoadedMessage(unsigned int aSender)
	: Message(eMessageType::NETWORK_LEVEL_LOADED)
	, mySender(aSender)
{
}