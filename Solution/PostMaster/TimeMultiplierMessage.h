#pragma once
#include "Message.h"

struct TimeMultiplierMessage : public Message
{
	TimeMultiplierMessage(eOwnerType aOwner, float aMultiplier);

	const eOwnerType myOwner;
	const float myMultiplier;
};

// takes in ratio, not actual position
inline TimeMultiplierMessage::TimeMultiplierMessage(eOwnerType aOwner, float aMultiplier)
	: Message(eMessageType::TIME_MULTIPLIER)
	, myOwner(aOwner)
	, myMultiplier(aMultiplier)
{
}