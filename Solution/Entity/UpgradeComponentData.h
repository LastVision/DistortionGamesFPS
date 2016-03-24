#pragma once
#include "GameEnum.h"

struct UpgradeComponentData
{
	bool myExistsInEntity = false;
	
	float myShootTimeModifier = 0.f;
	float myMinSpreadRotation = 0.f;
	float myMaxSpreadRotation = 0.f;

	int myClipSizeModifier = 0;
	int myAmmoTotalModifier = 0;
	int myDamageModifier = 0;
	
	eWeaponType myWeaponType;
};