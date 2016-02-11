#pragma once
#include "Message.h"
enum class eTutorialAction
{
	CLICK,
	ARTIFACT,
	ENEMY_BASE,
	RESOURCE_POINT,
	VICTORY_POINT,
};

struct TutorialMessage : public Message
{
	TutorialMessage(eTutorialAction aAction);

	const eTutorialAction myAction;
};

inline TutorialMessage::TutorialMessage(eTutorialAction aAction)
	: Message(eMessageType::TUTORIAL)
	, myAction(aAction)
{
}