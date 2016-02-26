#pragma once
#include "Message.h"
#include "Vector.h"

struct PostNetPositionMessage : public Message
{
	PostNetPositionMessage(const CU::Vector3<float>& aPosition, unsigned short anID);

	const CU::Vector3<float> myPosition;
	unsigned short myID;
};

inline PostNetPositionMessage::PostNetPositionMessage(const CU::Vector3<float>& aPosition, unsigned short anID)
	: Message(eMessageType::NETWORK_POSITION)
	, myPosition(aPosition)
	, myID(anID)
{
}