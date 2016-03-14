#pragma once
#include "Message.h"
struct NetworkOnHitMessage : public Message
{
	NetworkOnHitMessage(float aDamage, unsigned int aGID);

	float myDamage;
	unsigned int myGID;
};

inline NetworkOnHitMessage::NetworkOnHitMessage(float aDamage, unsigned int aGID)
	: Message(eMessageType::NETWORK_ON_HIT)
	, myDamage(aDamage)
	, myGID(aGID)
{
}

