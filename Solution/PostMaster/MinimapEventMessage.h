#pragma once
#include "Message.h"
#include "Vector.h"

enum class MinimapEventType
{
	eRESOURCE_POINT,
	eVICTORY_POINT,
	eUNIT_ATTACKED,
};

struct MinimapEventMessage : public Message
{
	MinimapEventMessage(const CU::Vector2<float>& aPosition, MinimapEventType aEventType);

	const CU::Vector2<float> myPosition;
	const MinimapEventType myEventType;
};

inline MinimapEventMessage::MinimapEventMessage(const CU::Vector2<float>& aPosition, MinimapEventType aEventType)
	: Message(eMessageType::MINIMAP_EVENT)
	, myPosition(aPosition)
	, myEventType(aEventType)
{
}