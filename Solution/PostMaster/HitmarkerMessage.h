#pragma once
#include "Message.h"

struct HitmarkerMessage : public Message
{
	HitmarkerMessage();
};

inline HitmarkerMessage::HitmarkerMessage()
	: Message(eMessageType::HITMARKER)
{
}

