#pragma once
#include "Message.h"
#include <string>

struct SendTextToClientsMessage : public Message
{
	SendTextToClientsMessage(const std::string& aText);

	const std::string& myText;
};

inline SendTextToClientsMessage::SendTextToClientsMessage(const std::string& aText)
	: Message(eMessageType::SEND_TEXT_TO_CLIENTS)
	, myText(aText)
{
}