#pragma once
#include "Message.h"
struct PostMasterNetRequestStartGameMessage : public Message
{
	PostMasterNetRequestStartGameMessage();
};

inline PostMasterNetRequestStartGameMessage::PostMasterNetRequestStartGameMessage()
	: Message(eMessageType::NETWORK_REQUEST_START_GAME)
{
}

