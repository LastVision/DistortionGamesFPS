#pragma once
#include "Message.h"

struct RespawnTriggerMessage : public Message
{
	RespawnTriggerMessage(unsigned int aGID);
	const int myGID;
};

inline RespawnTriggerMessage::RespawnTriggerMessage(unsigned int aGID)
	: Message(eMessageType::SET_ACTIVE)
	, myGID(aGID)
{
}