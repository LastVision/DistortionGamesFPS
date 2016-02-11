#pragma once
#include "Message.h"

namespace Prism
{
	class Scene;
}

struct KillUnitMessage : public Message
{
	KillUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aTargetPosition, int aAttacker);

	int myUnitType;
	int myOwnerType;
	int myAttackerOwnerType;
	const CU::Vector2<float> myTargetPosition;
};

inline KillUnitMessage::KillUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aTargetPosition, int aAttacker)
	: Message(eMessageType::KILL_UNIT)
	, myUnitType(aUnitType)
	, myOwnerType(aOwner)
	, myTargetPosition(aTargetPosition)
	, myAttackerOwnerType(aAttacker)
{
}