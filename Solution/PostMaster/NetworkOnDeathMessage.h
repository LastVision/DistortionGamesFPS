#pragma once
#include "Message.h"
struct NetworkOnDeathMessage : public Message
{
	NetworkOnDeathMessage(unsigned int aGID);

	unsigned int myGID;
};

inline NetworkOnDeathMessage::NetworkOnDeathMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_ON_DEATH)
	, myGID(aGID)
{
}

