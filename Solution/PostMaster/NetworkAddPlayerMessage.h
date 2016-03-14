#pragma once
#include "Message.h"

struct NetworkAddPlayerMessage : public Message
{
	NetworkAddPlayerMessage(unsigned int aGID, const sockaddr_in& anAddress);
	NetworkAddPlayerMessage(unsigned int aGID);

	unsigned int myGID;
	sockaddr_in myAddress;

};

inline NetworkAddPlayerMessage::NetworkAddPlayerMessage(unsigned int aGID, const sockaddr_in& anAddress)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myGID(aGID)
	, myAddress(anAddress)
{
}

inline NetworkAddPlayerMessage::NetworkAddPlayerMessage(unsigned int aGID)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myGID(aGID)
{
}