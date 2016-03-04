#include "stdafx.h"

#include <DamageNote.h>
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include "Weapon.h"


Weapon::Weapon(eWeaponType aWeaponType)
	: myWeaponType(aWeaponType)
{
}


Weapon::~Weapon()
{
}