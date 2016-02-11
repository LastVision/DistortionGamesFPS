#pragma once
#include "Message.h"
#include <Vector.h>
#include <string>

struct InWorldTextMessage : public Message
{
	InWorldTextMessage(const std::string& aText, const CU::Vector3<float>& aPosition
		, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });

	InWorldTextMessage(const std::string& aText, const CU::Vector2<float>& aPosition
		, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });

	const CU::Vector3<float> myPosition;
	const CU::Vector4<float> myColor;
	const std::string myText;
};

inline InWorldTextMessage::InWorldTextMessage(const std::string& aText, const CU::Vector3<float>& aPosition
	, const CU::Vector4<float>& aColor)
	: Message(eMessageType::IN_WORLD_TEXT)
	, myPosition(aPosition)
	, myColor(aColor)
	, myText(aText)
{
}

inline InWorldTextMessage::InWorldTextMessage(const std::string& aText, const CU::Vector2<float>& aPosition
	, const CU::Vector4<float>& aColor)
	: Message(eMessageType::IN_WORLD_TEXT)
	, myPosition({ aPosition.x, 0.f, aPosition.y })
	, myColor(aColor)
	, myText(aText)
{
}