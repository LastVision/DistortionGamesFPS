#pragma once
#include "Message.h"

struct NetworkRemovePlayerMessage : public Message
{
	NetworkRemovePlayerMessage(unsigned int aGID);

	unsigned int myGID;

};

inline NetworkRemovePlayerMessage::NetworkRemovePlayerMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_REMOVE_PLAYER)
	, myGID(aGID)
{
}