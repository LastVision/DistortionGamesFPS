#pragma once
#include "Message.h"
struct PostMasterNetOnDisconnectMessage : public Message
{
	PostMasterNetOnDisconnectMessage(unsigned int aGID);

	unsigned int myGID;
};

inline PostMasterNetOnDisconnectMessage::PostMasterNetOnDisconnectMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_ON_DISCONNECT)
	, myGID(aGID)
{
}

