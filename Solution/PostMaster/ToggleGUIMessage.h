#pragma once
#include "Message.h"

struct ToggleGUIMessage : public Message
{
	ToggleGUIMessage(bool aShowGUI, float aFadeTime);

	const bool myShowGUI;
	const float myFadeTime;

};

inline ToggleGUIMessage::ToggleGUIMessage(bool aShowGUI, float aFadeTime)
	: Message(eMessageType::TOGGLE_GUI)
	, myShowGUI(aShowGUI)
	, myFadeTime(aFadeTime)
{
}