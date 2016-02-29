#include "stdafx.h"

#include <DamageNote.h>
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include "Weapon.h"

Weapon::Weapon(eWeaponType aWeaponType)
	: myWeaponType(aWeaponType)
{
	if (myWeaponType == eWeaponType::PISTOL)
	{
		myClipSize = 12;
		myAmmoInClip = myClipSize;
		myAmmoTotal = INT_MAX;
		myDamage = 10;
	}
	else if (myWeaponType == eWeaponType::SHOTGUN)
	{
		myClipSize = 8;
		myAmmoInClip = myClipSize;
		myAmmoTotal = 64;
		myDamage = 20;
	}
	else if (myWeaponType == eWeaponType::GRENADE_LAUNCHER)
	{
		myClipSize = 5;
		myAmmoInClip = myClipSize;
		myAmmoTotal = 10;
		myDamage = 50;
	}
	else
	{
		DL_ASSERT("Unknown weapontype");
	}
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
			ShootRowAround(aOrientation, aOrientation.GetForward() * CU::Matrix44<float>::CreateRotateAroundX(-0.25f));
			ShootRowAround(aOrientation, aOrientation.GetForward());
			ShootRowAround(aOrientation, aOrientation.GetForward() * CU::Matrix44<float>::CreateRotateAroundX(0.25f));
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
		, forward * CU::Matrix44<float>::CreateRotateAroundY(-0.25f), 100.f);
	if (entity != nullptr)
	{
		if (entity->GetPhysEntity()->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			entity->GetPhysEntity()->AddForce(aOrientation.GetForward(), 25.f);
		}
		entity->SendNote<DamageNote>(DamageNote(myDamage));
	}

	entity = Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
		, forward * CU::Matrix44<float>::CreateRotateAroundY(0.25f), 100.f);
	if (entity != nullptr)
	{
		if (entity->GetPhysEntity()->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			entity->GetPhysEntity()->AddForce(aOrientation.GetForward(), 25.f);
		}
		entity->SendNote<DamageNote>(DamageNote(myDamage));
	}
}
