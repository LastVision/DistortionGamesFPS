#pragma once

#include "MessageEnum.h"

struct Message
{
	Message(eMessageType aMessageType);
	eMessageType GetMessageType() const;


	void operator=(Message&) = delete;
	const eMessageType myMessageType;
};

inline Message::Message(eMessageType aMessageType)
	:myMessageType(aMessageType)
{
}

inline eMessageType Message::GetMessageType() const
{
	return myMessageType;
}