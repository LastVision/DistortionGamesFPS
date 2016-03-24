#pragma once
#include "Message.h"


struct ActivateSpawnpointMessage : public Message
{
	ActivateSpawnpointMessage(unsigned int aGID);
	const unsigned int myGID;
};

inline ActivateSpawnpointMessage::ActivateSpawnpointMessage(unsigned int aGID)
	: Message(eMessageType::ACTIVATE_SPAWNPOINT)
	, myGID(aGID)
{
}

