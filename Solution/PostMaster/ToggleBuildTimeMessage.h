#pragma once
#include "Message.h"

struct ToggleBuildTimeMessage : public Message
{
	ToggleBuildTimeMessage(bool aIgnoreBuildTime);

	const bool myIgnoreBuildTime;
};

// takes in ratio, not actual position
inline ToggleBuildTimeMessage::ToggleBuildTimeMessage(bool aIgnoreBuildTime)
	: Message(eMessageType::TOGGLE_BUILD_TIME)
	, myIgnoreBuildTime(aIgnoreBuildTime)
{
}