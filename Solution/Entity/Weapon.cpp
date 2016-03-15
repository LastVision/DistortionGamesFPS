#include "stdafx.h"

#include "Weapon.h"
#include "UpgradeComponentData.h"

Weapon::Weapon(eWeaponType aWeaponType)
	: myWeaponType(aWeaponType)
	, myForceStrength(0.f)
{
}


Weapon::~Weapon()
{
}

void Weapon::Upgrade(const UpgradeComponentData& aData)
{
	myAmmoTotal += aData.myAmmoTotalModifier;
	myClipSize += aData.myClipSizeModifier;
	myDamage += aData.myDamageModifier;
	myShootTime += aData.myShootTimeModifier;
}