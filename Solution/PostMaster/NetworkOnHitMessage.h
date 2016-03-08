#pragma once
#include "Message.h"
struct NetworkOnHitMessage : public Message
{
	NetworkOnHitMessage(float aDamage, unsigned int aNetworkID);

	float myDamage;
	unsigned int myNetworkID;
};

inline NetworkOnHitMessage::NetworkOnHitMessage(float aDamage, unsigned int aNetworkID)
	: Message(eMessageType::NETWORK_ON_HIT)
	, myDamage(aDamage)
	, myNetworkID(aNetworkID)
{
}

