#pragma once
#include "Message.h"
struct NetworkOnDisconnectMessage : public Message
{
	NetworkOnDisconnectMessage(unsigned int aNetworkID);

	unsigned int myNetworkID;
};

inline NetworkOnDisconnectMessage::NetworkOnDisconnectMessage(unsigned int aNetworkID)
	: Message(eMessageType::NETWORK_ON_DISCONNECT)
	, myNetworkID(aNetworkID)
{
}

