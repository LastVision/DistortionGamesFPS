#pragma once
#include "Message.h"

struct LevelCompleteMessage : public Message
{
	LevelCompleteMessage(int aLevelID);

	const int myLevelID;
};

inline LevelCompleteMessage::LevelCompleteMessage(int aLevelID)
	: Message(eMessageType::LEVEL_COMPLETE)
	, myLevelID(aLevelID)
{
}