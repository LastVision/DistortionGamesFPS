#pragma once
#include "GameEnum.h"

struct UpgradeComponentData
{
	bool myExistsInEntity = false;
	
	float myShootTimeModifier;
	float myMinSpreadRotation;
	float myMaxSpreadRotation;

	int myClipSizeModifier;
	int myAmmoTotalModifier;
	int myDamageModifier;
	
	eWeaponType myWeaponType;
};