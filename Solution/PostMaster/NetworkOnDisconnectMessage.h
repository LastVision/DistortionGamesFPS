#pragma once
#include "Message.h"
struct NetworkOnDisconnectMessage : public Message
{
	NetworkOnDisconnectMessage(unsigned int aGID);

	unsigned int myGID;
};

inline NetworkOnDisconnectMessage::NetworkOnDisconnectMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_ON_DISCONNECT)
	, myGID(aGID)
{
}

