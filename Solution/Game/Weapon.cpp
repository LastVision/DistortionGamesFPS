#include "stdafx.h"
#include "Weapon.h"


Weapon::Weapon(eWeaponType aWeaponType)
	: myWeaponType(aWeaponType)
{
	if (myWeaponType == eWeaponType::PISTOL)
	{
		myClipSize = 12;
		myAmmoInClip = myClipSize;
		myAmmoTotal = INT_MAX;
		myDamage = 10.f;
	}
	else if (myWeaponType == eWeaponType::SHOTGUN)
	{
		//DL_ASSERT("Shotgun not implemented");
	}
	else if (myWeaponType == eWeaponType::GRENADE_LAUNCHER)
	{
		//DL_ASSERT("GrenadeLauncher not implemented");
	}
	else
	{
		DL_ASSERT("Unknown weapontype");
	}
}


Weapon::~Weapon()
{
}

void Weapon::Shoot()
{
	if (myAmmoInClip > 0)
	{
		myAmmoInClip -= 1;
	}
}

void Weapon::Reload()
{
	if (myWeaponType == eWeaponType::PISTOL)
	{
		myAmmoInClip = myClipSize;
	}
	else
	{
		int ammoLeft = myAmmoTotal;
		myAmmoTotal -= min(ammoLeft, myClipSize - myAmmoInClip);
		myAmmoInClip = ammoLeft;
	}
}
