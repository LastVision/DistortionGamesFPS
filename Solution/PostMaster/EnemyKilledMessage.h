#pragma once
#include "Message.h"
class Entity;

struct EnemyKilledMessage : public Message
{
	EnemyKilledMessage();
	EnemyKilledMessage(const Entity* aEnemy);

	const Entity* myEnemy;
};

inline EnemyKilledMessage::EnemyKilledMessage()
	: Message(eMessageType::ENEMY_KILLED)
	, myEnemy(nullptr)
{
}

inline EnemyKilledMessage::EnemyKilledMessage(const Entity* aEnemy)
	: Message(eMessageType::ENEMY_KILLED)
	, myEnemy(aEnemy)
{
}