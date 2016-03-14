#pragma once
#include "Message.h"

struct PostMasterNetAddPlayerMessage : public Message
{
	PostMasterNetAddPlayerMessage(unsigned int aGID, const sockaddr_in& anAddress);
	PostMasterNetAddPlayerMessage(unsigned int aGID);

	unsigned int myGID;
	sockaddr_in myAddress;

};

inline PostMasterNetAddPlayerMessage::PostMasterNetAddPlayerMessage(unsigned int aGID, const sockaddr_in& anAddress)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myGID(aGID)
	, myAddress(anAddress)
{
}

inline PostMasterNetAddPlayerMessage::PostMasterNetAddPlayerMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myGID(aGID)
{
}