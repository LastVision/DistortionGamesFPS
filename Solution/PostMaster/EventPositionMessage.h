#pragma once
#include "Message.h"
#include "Vector.h"

struct EventPositionMessage : public Message
{
	EventPositionMessage(const CU::Vector2<float>& aPosition);

	const CU::Vector2<float> myPosition;
};

inline EventPositionMessage::EventPositionMessage(const CU::Vector2<float>& aPosition)
	: Message(eMessageType::EVENT_POSITION)
	, myPosition(aPosition)
{
}