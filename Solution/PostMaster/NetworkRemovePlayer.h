#pragma once
#include "Message.h"

struct NetworkRemovePlayerMessage : public Message
{
	NetworkRemovePlayerMessage(unsigned short aNetworkID);

	unsigned short myNetworkID;

};

inline NetworkRemovePlayerMessage::NetworkRemovePlayerMessage(unsigned short aNetworkID)
	: Message(eMessageType::NETWORK_REMOVE_PLAYER)
	, myNetworkID(aNetworkID)
{
}