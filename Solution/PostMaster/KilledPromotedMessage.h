#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <Vector.h>

struct KilledPromotedMessage : public Message
{
	KilledPromotedMessage(eOwnerType aOwner, int aGunpowderValue, const CU::Vector2<float>& aUnitPosition);

	const eOwnerType myOwner;
	const int myGunpowderModifier;
	const CU::Vector2<float> myUnitPosition;
};

inline KilledPromotedMessage::KilledPromotedMessage(eOwnerType aOwner, int aGunPowderValue, const CU::Vector2<float>& aUnitPosition)
	: Message(eMessageType::KILLED_PROMOTION)
	, myOwner(aOwner)
	, myGunpowderModifier(aGunPowderValue)
	, myUnitPosition(aUnitPosition)
{
}