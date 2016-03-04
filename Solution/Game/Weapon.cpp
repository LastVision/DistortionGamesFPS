#include "stdafx.h"

#include <DamageNote.h>
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include "Weapon.h"


Weapon::Weapon(eWeaponType aWeaponType)
	: myWeaponType(aWeaponType)
{
	myRaycastHandler = [=](Entity* anEntity, const CU::Vector3<float>& aDirection)
	{
		this->HandleRaycast(anEntity, aDirection);
	};
}


Weapon::~Weapon()
{
}