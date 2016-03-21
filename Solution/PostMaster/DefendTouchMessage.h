#pragma once
#include "Message.h"

struct DefendTouchMessage : public Message
{
	DefendTouchMessage(bool aHasEntered);
	const bool myHasEntered;
};

inline DefendTouchMessage::DefendTouchMessage(bool aHasEntered)
	: Message(eMessageType::ENEMY_KILLED)
	, myHasEntered(aHasEntered)
{
}