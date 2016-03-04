#include "stdafx.h"

#include <DamageNote.h>
#include <Entity.h>
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include "Pistol.h"
#include <XMLReader.h>

Pistol::Pistol()
	: Weapon(eWeaponType::PISTOL)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	tinyxml2::XMLElement* pistolElement = reader.ForceFindFirstChild(root, "pistol");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "clipsize"), "value", myClipSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "damage"), "value", myDamage);

	myAmmoInClip = myClipSize;
	myAmmoTotal = INT_MAX;

	reader.CloseDocument();
}

Pistol::~Pistol()
{
}


void Pistol::Shoot(const CU::Matrix44<float>& aOrientation)
{
	if (myAmmoInClip > 0)
	{
		Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
			, aOrientation.GetForward(), 500.f, myRaycastHandler);
		myAmmoInClip -= 1;
	}
}

void Pistol::Reload()
{
	myAmmoInClip = myClipSize;
}

void Pistol::HandleRaycast(Entity* anEntity, const CU::Vector3<float>& aDirection)
{
	if (anEntity != nullptr)
	{
		if (anEntity->GetPhysEntity()->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			anEntity->GetPhysEntity()->AddForce(aDirection, 25.f);
		}

		anEntity->SendNote<DamageNote>(DamageNote(myDamage));

	}
}