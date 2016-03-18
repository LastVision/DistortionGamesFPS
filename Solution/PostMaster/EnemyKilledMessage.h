#pragma once
#include "Message.h"

struct EnemyKilledMessage : public Message
{
	EnemyKilledMessage();
};

inline EnemyKilledMessage::EnemyKilledMessage()
	: Message(eMessageType::ENEMY_KILLED)
{
}