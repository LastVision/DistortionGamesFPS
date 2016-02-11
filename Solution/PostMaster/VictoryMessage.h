#pragma once
#include "Message.h"

class Entity;


struct VictoryMessage : public Message
{
	VictoryMessage(eOwnerType aOwner, int aVictoryModifier);

	const eOwnerType myOwner;
	const int myVictoryModifier;
};

inline VictoryMessage::VictoryMessage(eOwnerType aOwner, int aVictoryModifier)
	: Message(eMessageType::VICTORY)
	, myOwner(aOwner)
	, myVictoryModifier(aVictoryModifier)
{
}