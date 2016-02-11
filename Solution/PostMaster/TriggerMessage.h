#pragma once
#include "Message.h"

class Entity;


struct TriggerMessage : public Message
{
	enum class eTriggerType
	{
		EXIT,
		ENTER,
	};
	TriggerMessage(Entity* aTrigger, Entity* aUnit, eTriggerType aType);

	const eTriggerType myType;
	const Entity* myTrigger;
	const Entity* myUnit;
};

inline TriggerMessage::TriggerMessage(Entity* aTrigger, Entity* aUnit, eTriggerType aType)
	: Message(eMessageType::TRIGGER)
	, myTrigger(aTrigger)
	, myUnit(aUnit)
	, myType(aType)
{
}