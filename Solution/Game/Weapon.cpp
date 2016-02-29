#include "stdafx.h"

#include <DamageNote.h>
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include "Weapon.h"
#include <XMLReader.h>

Weapon::Weapon(eWeaponType aWeaponType)
	: myWeaponType(aWeaponType)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	if (myWeaponType == eWeaponType::PISTOL)
	{
		tinyxml2::XMLElement* pistolElement = reader.ForceFindFirstChild(root, "pistol");
		
		reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "clipsize"), "value", myClipSize);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "damage"), "value", myDamage);
		
		myAmmoInClip = myClipSize;
		myAmmoTotal = INT_MAX;
	}
	else if (myWeaponType == eWeaponType::SHOTGUN)
	{
		tinyxml2::XMLElement* shotgunElement = reader.ForceFindFirstChild(root, "shotgun");

		reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "clipsize"), "value", myClipSize);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "damage"), "value", myDamage);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "startammo"), "value", myAmmoTotal);
		myAmmoInClip = myClipSize;
	}
	else if (myWeaponType == eWeaponType::GRENADE_LAUNCHER)
	{
		tinyxml2::XMLElement* grenadeLauncherElement = reader.ForceFindFirstChild(root, "grenadelauncher");
		reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "clipsize"), "value", myClipSize);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "damage"), "value", myDamage);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "startammo"), "value", myAmmoTotal);
		myAmmoInClip = myClipSize;
	}
	else
	{
		DL_ASSERT("Unknown weapontype");
	}
	reader.CloseDocument();
}


Weapon::~Weapon()
{
}

void Weapon::Shoot(const CU::Matrix44<float>& aOrientation)
{
	if (myAmmoInClip > 0)
	{
		if (myWeaponType == eWeaponType::PISTOL)
		{
			Entity* entity = Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
				, aOrientation.GetForward(), 500.f);

			if (entity != nullptr)
			{
				if (entity->GetPhysEntity()->GetPhysicsType() == ePhysics::DYNAMIC)
				{
					entity->GetPhysEntity()->AddForce(aOrientation.GetForward(), 25.f);
				}

				entity->SendNote<DamageNote>(DamageNote(myDamage));

			}
		}
		else if (myWeaponType == eWeaponType::SHOTGUN)
		{
			ShootRowAround(aOrientation, aOrientation.GetForward() * CU::Matrix44<float>::CreateRotateAroundX(CU::Math::RandomRange(-0.25f, -0.01f)));
			ShootRowAround(aOrientation, aOrientation.GetForward());
			ShootRowAround(aOrientation, aOrientation.GetForward() * CU::Matrix44<float>::CreateRotateAroundX(CU::Math::RandomRange(0.01f, 0.25f)));
		}
		else if (myWeaponType == eWeaponType::GRENADE_LAUNCHER)
		{

		}
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
		myAmmoInClip = myClipSize;
	}
}

void Weapon::ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward)
{
	CU::Vector3<float> forward = aForward;

	Entity* entity = Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
		, forward, 100.f);
	if (entity != nullptr)
	{
		if (entity->GetPhysEntity()->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			entity->GetPhysEntity()->AddForce(aOrientation.GetForward(), 25.f);
		}
		entity->SendNote<DamageNote>(DamageNote(myDamage));
	}

	entity = Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
		, forward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(-0.25f, -0.01f)), 100.f);
	if (entity != nullptr)
	{
		if (entity->GetPhysEntity()->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			entity->GetPhysEntity()->AddForce(aOrientation.GetForward(), 25.f);
		}
		entity->SendNote<DamageNote>(DamageNote(myDamage));
	}

	entity = Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
		, forward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(0.01f, 0.25f)), 100.f);
	if (entity != nullptr)
	{
		if (entity->GetPhysEntity()->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			entity->GetPhysEntity()->AddForce(aOrientation.GetForward(), 25.f);
		}
		entity->SendNote<DamageNote>(DamageNote(myDamage));
	}
}
