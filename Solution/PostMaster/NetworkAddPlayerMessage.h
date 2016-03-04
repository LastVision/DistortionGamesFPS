#pragma once
#include "Message.h"

struct NetworkAddPlayerMessage : public Message
{
	NetworkAddPlayerMessage(unsigned short aNetworkID);

	unsigned short myNetworkID;

};

inline NetworkAddPlayerMessage::NetworkAddPlayerMessage(unsigned short aNetworkID)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myNetworkID(aNetworkID)
{
}

