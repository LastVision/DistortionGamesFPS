#pragma once
#include "Message.h"
#include <string>

struct SendTextToClientsMessage : public Message
{
	SendTextToClientsMessage(const std::string& aText, float aTime = 5.f);

	const std::string& myText;
	const float myTime;
};

inline SendTextToClientsMessage::SendTextToClientsMessage(const std::string& aText
	, float aTime)
	: Message(eMessageType::SEND_TEXT_TO_CLIENTS)
	, myText(aText)
	, myTime(aTime)
{
}