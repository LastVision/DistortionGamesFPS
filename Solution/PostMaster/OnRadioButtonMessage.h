#pragma once
#include "Message.h"

enum class eOnRadioButtonEvent
{
	NONE,
	LEVEL_SELECT,
	DIFFICULTY_SELECT,
};

struct OnRadioButtonMessage : public Message
{
	OnRadioButtonMessage(eOnRadioButtonEvent anEvent, int anID);

	const eOnRadioButtonEvent myEvent;
	const int myID;
};

inline OnRadioButtonMessage::OnRadioButtonMessage(eOnRadioButtonEvent anEvent, int anID)
	: Message(eMessageType::ON_RADIO_BUTTON)
	, myEvent(anEvent)
	, myID(anID)
{
}