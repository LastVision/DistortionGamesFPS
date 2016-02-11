#pragma once
#include "Message.h"
#include "Vector.h"

struct MinimapMoveMessage : public Message
{
	MinimapMoveMessage(CU::Vector2<float> aPosition);

	const CU::Vector2<float> myPosition;
};

inline MinimapMoveMessage::MinimapMoveMessage(CU::Vector2<float> aPosition)
	: Message(eMessageType::MOVE_UNITS)
	, myPosition(aPosition)
{
}