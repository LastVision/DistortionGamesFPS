#pragma once
#include "Message.h"

namespace Prism
{
	class Text;
}

struct TextMessage : public Message
{
	TextMessage(Prism::Text* aText, bool aVisibleFlag = true);

	Prism::Text* myText;
	bool myVisibleFlag;
};

inline TextMessage::TextMessage(Prism::Text* aText, bool aVisibleFlag)
	: Message(eMessageType::TEXT)
	, myText(aText)
	, myVisibleFlag(aVisibleFlag)
{
}