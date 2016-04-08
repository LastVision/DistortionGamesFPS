#pragma once
#include "Message.h"

struct LevelLoadedMessage : public Message
{
	LevelLoadedMessage(int aLevelID);

	const int myLevelID;
};

inline LevelLoadedMessage::LevelLoadedMessage(int aLevelID)
	: Message(eMessageType::LEVEL_LOADED)
	, myLevelID(aLevelID)
{
}