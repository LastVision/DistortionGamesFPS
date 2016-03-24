#pragma once
#include "Message.h"

struct RespawnTriggerMessage : public Message
{
	RespawnTriggerMessage(unsigned int aGID);
	const int myGID;
};

inline RespawnTriggerMessage::RespawnTriggerMessage(unsigned int aGID)
	: Message(eMessageType::RESPAWN_TRIGGER)
	, myGID(aGID)
{
}