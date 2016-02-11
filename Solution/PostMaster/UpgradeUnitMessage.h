#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <GameStructs.h>

struct UpgradeUnitMessage : public Message
{
	UpgradeUnitMessage(eUnitType aUnit, const Upgrade& aUpgrade, eOwnerType aOwner, int aUpgradeLevel);

	const eUnitType myUnit;
	const Upgrade& myUpgrade;
	const eOwnerType myOwner;
	const int myUpgradeLevel;
};

inline UpgradeUnitMessage::UpgradeUnitMessage(eUnitType aUnit, const Upgrade& aUpgrade, eOwnerType aOwner, int aUpgradeLevel)
	: Message(eMessageType::UPGRADE_UNIT)
	, myUnit(aUnit)
	, myUpgrade(aUpgrade)
	, myOwner(aOwner)
	, myUpgradeLevel(aUpgradeLevel)
{
}