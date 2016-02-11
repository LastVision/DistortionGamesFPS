#pragma once
#include "Message.h"

struct ToggleFogOfWarMessage : public Message
{
	ToggleFogOfWarMessage();
};

inline ToggleFogOfWarMessage::ToggleFogOfWarMessage()
	: Message(eMessageType::TOGGLE_FOG_OF_WAR)
{
}