#pragma once
#include "Message.h"

struct FadeMessage : public Message
{
	FadeMessage(float aDuration);

	const float myDuration;
};

// takes in ratio, not actual position
inline FadeMessage::FadeMessage(float aDuration)
	: Message(eMessageType::FADE)
	, myDuration(aDuration)
{
}