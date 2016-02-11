#pragma once
#include "Message.h"
#include "Vector.h"

enum class eHowToHandleMovement
{
	ZERO_TO_ONE,
	WORLD_POSITION
};

struct MoveCameraMessage : public Message
{
	MoveCameraMessage(const CU::Vector2<float>& aPosition, eHowToHandleMovement aHowToHandleMovement);

	const CU::Vector2<float> myPosition;
	const eHowToHandleMovement myMovementHandle;
};

// takes in ratio, not actual position
inline MoveCameraMessage::MoveCameraMessage(const CU::Vector2<float>& aPosition, eHowToHandleMovement aHowToHandleMovement)
	: Message(eMessageType::MOVE_CAMERA)
	, myPosition(aPosition)
	, myMovementHandle(aHowToHandleMovement)
{
}