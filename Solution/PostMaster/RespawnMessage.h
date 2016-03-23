#pragma once
#include "Message.h"

struct RespawnMessage : public Message
{
	RespawnMessage(int aGID);

	int myGID;
};

inline RespawnMessage::RespawnMessage(int aGID)
	: Message(eMessageType::RESPAWN)
	, myGID(aGID)
{
}