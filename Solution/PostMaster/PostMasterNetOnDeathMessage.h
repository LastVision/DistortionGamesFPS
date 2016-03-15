#pragma once
#include "Message.h"
struct PostMasterNetOnDeathMessage : public Message
{
	PostMasterNetOnDeathMessage(unsigned int aGID);

	unsigned int myGID;
};

inline PostMasterNetOnDeathMessage::PostMasterNetOnDeathMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_ON_DEATH)
	, myGID(aGID)
{
}

