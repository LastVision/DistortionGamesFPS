#pragma once
#include "Message.h"

struct ToggleRenderLinesMessage : public Message
{
	ToggleRenderLinesMessage(bool aToggleFlag);

	const bool myToggleFlag;
};

inline ToggleRenderLinesMessage::ToggleRenderLinesMessage(bool aToggleFlag)
	: Message(eMessageType::TOGGLE_RENDER_LINES)
	, myToggleFlag(aToggleFlag)
{
}