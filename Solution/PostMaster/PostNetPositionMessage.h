#pragma once
#include "Message.h"
#include "Vector.h"

struct PostNetPositionMessage : public Message
{
	PostNetPositionMessage(const CU::Vector3<float>& aPosition);

	const CU::Vector3<float> myPosition;
};

inline PostNetPositionMessage::PostNetPositionMessage(const CU::Vector3<float>& aPosition)
	: Message(eMessageType::NETWORK_POSITION)
	, myPosition(aPosition)
{
}