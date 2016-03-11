#pragma once
#include "GameEnum.h"

struct UpgradeComponentData
{
	bool myExistsInEntity = false;
	
	float myShootTimeModifier;
	int myClipSizeModifier;
	int myAmmoTotalModifier;
	int myDamageModifier;
	
	eWeaponType myWeaponType;
};