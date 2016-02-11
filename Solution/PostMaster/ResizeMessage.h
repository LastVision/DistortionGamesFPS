#pragma once
#include "Message.h"
#include <Vector.h>

struct ResizeMessage : public Message
{
	ResizeMessage(int aWidth, int aHeight);

	CU::Vector2<int> myResolution;
};

inline ResizeMessage::ResizeMessage(int aWidth, int aHeight)
	: Message(eMessageType::RESIZE)
	, myResolution(aWidth, aHeight)
{
}