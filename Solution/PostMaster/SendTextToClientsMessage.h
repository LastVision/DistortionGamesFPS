#pragma once
#include "Message.h"
#include <string>

struct SendTextToClientsMessage : public Message
{
	SendTextToClientsMessage(const std::string& aText, float aTime = 5.f, int aTextRow = 0);

	const std::string& myText;
	const float myTime;
	const int myTextRows;
};

inline SendTextToClientsMessage::SendTextToClientsMessage(const std::string& aText
	, float aTime, int aTextRow)
	: Message(eMessageType::SEND_TEXT_TO_CLIENTS)
	, myText(aText)
	, myTime(aTime)
	, myTextRows(aTextRow)
{
}