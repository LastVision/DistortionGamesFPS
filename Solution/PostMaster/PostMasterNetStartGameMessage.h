#pragma once
#include "Message.h"
struct PostMasterNetStartGameMessage : public Message
{
	PostMasterNetStartGameMessage(int aLevelID);

	int myLevelID;
};

inline PostMasterNetStartGameMessage::PostMasterNetStartGameMessage(int aLevelID)
	: Message(eMessageType::NETWORK_START_GAME)
	, myLevelID(aLevelID)
{
}