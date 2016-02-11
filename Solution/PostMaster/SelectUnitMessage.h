#pragma once
#include "Message.h"

struct SelectUnitMessage : public Message
{
	SelectUnitMessage(int aUnitIndex);

	const int myUnitIndex;
};

inline SelectUnitMessage::SelectUnitMessage(int aUnitIndex)
	: Message(eMessageType::SELECT_UNIT)
	, myUnitIndex(aUnitIndex)
{
}