#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <Vector.h>

struct BlockMapMessage : public Message
{
	BlockMapMessage(const CU::Vector2<float>& aPosition, bool aAddValue);

	const CU::Vector2<float>& myPosition;
	const bool myShouldAddValue;
};

inline BlockMapMessage::BlockMapMessage(const CU::Vector2<float>& aPosition, bool aAddValue)
	: Message(eMessageType::BLOCK_MAP)
	, myPosition(aPosition)
	, myShouldAddValue(aAddValue)
{
}