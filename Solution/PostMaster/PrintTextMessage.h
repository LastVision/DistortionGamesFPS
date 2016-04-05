#pragma once
#include "Message.h"
#include <string>

struct PrintTextMessage : public Message
{
	PrintTextMessage(const std::string& aText, float aTime = 5.f);

	const std::string myText;
	const float myTime;
};

inline PrintTextMessage::PrintTextMessage(const std::string& aText, float aTime)
	: Message(eMessageType::PRINT_TEXT)
	, myText(aText)
	, myTime(aTime)
{
}
