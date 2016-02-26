#pragma once

#include "Message.h"

struct PostNetConnectMessage : public Message
{
	PostNetConnectMessage();

};

inline PostNetConnectMessage::PostNetConnectMessage()
	: Message(eMessageType::NETWORK_ON_CONNECT)
{
}