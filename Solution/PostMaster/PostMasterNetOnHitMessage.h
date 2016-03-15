#pragma once
#include "Message.h"
struct PostMasterNetOnHitMessage : public Message
{
	PostMasterNetOnHitMessage(float aDamage, unsigned int aGID);

	float myDamage;
	unsigned int myGID;
};

inline PostMasterNetOnHitMessage::PostMasterNetOnHitMessage(float aDamage, unsigned int aGID)
	: Message(eMessageType::NETWORK_ON_HIT)
	, myDamage(aDamage)
	, myGID(aGID)
{
}

