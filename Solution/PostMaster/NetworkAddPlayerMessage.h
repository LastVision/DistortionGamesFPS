#pragma once
#include "Message.h"

struct NetworkAddPlayerMessage : public Message
{
	NetworkAddPlayerMessage(unsigned short aNetworkID, const sockaddr_in& anAddress);
	NetworkAddPlayerMessage(unsigned short aNetworkID);

	unsigned short myNetworkID;
	sockaddr_in myAddress;

};

inline NetworkAddPlayerMessage::NetworkAddPlayerMessage(unsigned short aNetworkID, const sockaddr_in& anAddress)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myNetworkID(aNetworkID)
	, myAddress(anAddress)
{
}

inline NetworkAddPlayerMessage::NetworkAddPlayerMessage(unsigned short aNetworkID)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myNetworkID(aNetworkID)
{
}