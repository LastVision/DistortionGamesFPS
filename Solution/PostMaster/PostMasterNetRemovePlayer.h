#pragma once
#include "Message.h"

struct PostMasterNetRemovePlayerMessage : public Message
{
	PostMasterNetRemovePlayerMessage(unsigned int aGID);

	unsigned int myGID;

};

inline PostMasterNetRemovePlayerMessage::PostMasterNetRemovePlayerMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_REMOVE_PLAYER)
	, myGID(aGID)
{
}