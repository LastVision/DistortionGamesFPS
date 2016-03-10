#pragma once
#include "Message.h"
struct NetworkOnDeathMessage : public Message
{
	NetworkOnDeathMessage(unsigned int aNetworkID);

	unsigned int myNetworkID;
};

inline NetworkOnDeathMessage::NetworkOnDeathMessage(unsigned int aNetworkID)
	: Message(eMessageType::NETWORK_ON_DEATH)
	, myNetworkID(aNetworkID)
{
}

