#pragma once
#include "Message.h"
#include <Vector.h>
#include <string>

struct NotificationMessage : public Message
{
	NotificationMessage(const std::string& aText, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });

	const CU::Vector4<float> myColor;
	const std::string myText;
};

inline NotificationMessage::NotificationMessage(const std::string& aText, const CU::Vector4<float>& aColor)
	: Message(eMessageType::NOTIFICATION)
	, myColor(aColor)
	, myText(aText)
{
}
