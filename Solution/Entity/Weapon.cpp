#include "stdafx.h"
#include "UpgradeComponentData.h"
#include "Weapon.h"

Weapon::Weapon(eWeaponType aWeaponType, std::string aXMLTagName, Entity* aOwnerEntity)
	: myWeaponType(aWeaponType)
	, myForceStrength(0.f)
	, myOwnerEntity(aOwnerEntity)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	tinyxml2::XMLElement* grenadeLauncherElement = reader.ForceFindFirstChild(root, aXMLTagName);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "clipsize"), "value", myClipSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "damage"), "value", myDamage);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "shoottime"), "value", myShootTime);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "forceStrength"), "value", myForceStrength);

	reader.CloseDocument();

	myAmmoInClip = myClipSize;
	myShootTimer = myShootTime;
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