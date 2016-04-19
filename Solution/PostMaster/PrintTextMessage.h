#pragma once
#include "Message.h"
#include <string>

struct PrintTextMessage : public Message
{
	PrintTextMessage(const std::string& aText, float aTime = 5.f, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f }
	, int aTextRows = 0);

	const std::string myText;
	const float myTime;
	const CU::Vector4<float> myColor;
	const int myTextRows;
};

inline PrintTextMessage::PrintTextMessage(const std::string& aText, float aTime, const CU::Vector4<float>& aColor,
	int aTextRows)
	: Message(eMessageType::PRINT_TEXT)
	, myText(aText)
	, myTime(aTime)
	, myColor(aColor)
	, myTextRows(aTextRows)
{
}
