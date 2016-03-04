#include "stdafx.h"

#include <DamageNote.h>
#include <Entity.h>
#include "GrenadeLauncher.h"
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include <XMLReader.h>


GrenadeLauncher::GrenadeLauncher()
	: Weapon(eWeaponType::GRENADE_LAUNCHER)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	tinyxml2::XMLElement* grenadeLauncherElement = reader.ForceFindFirstChild(root, "grenadelauncher");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "clipsize"), "value", myClipSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "damage"), "value", myDamage);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "startammo"), "value", myAmmoTotal);
	myAmmoInClip = myClipSize;

	reader.CloseDocument();
}


GrenadeLauncher::~GrenadeLauncher()
{
}


void GrenadeLauncher::Shoot(const CU::Matrix44<float>& aOrientation)
{
	if (myAmmoInClip > 0)
	{
		myAmmoInClip -= 1;
	}
}
void GrenadeLauncher::Reload()
{
	int ammoLeft = myAmmoTotal;
	myAmmoTotal -= min(ammoLeft, myClipSize - myAmmoInClip);
	myAmmoInClip = myClipSize;
}