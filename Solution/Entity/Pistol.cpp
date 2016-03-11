#include "stdafx.h"

#include "DamageNote.h"
#include "Entity.h"
#include <EmitterMessage.h>
#include <NetworkOnHitMessage.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
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
	reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "shoottime"), "value", myShootTime);

	myAmmoInClip = myClipSize;
	myAmmoTotal = INT_MAX;
	myShootTimer = myShootTime;

	reader.CloseDocument();

	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition);
	};
}

Pistol::~Pistol()
{
}


bool Pistol::Shoot(const CU::Matrix44<float>& aOrientation)
{
	if (myAmmoInClip > 0 && myShootTimer <= 0.f)
	{
		Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
			, aOrientation.GetForward(), 500.f, myRaycastHandler);
		myAmmoInClip -= 1;
		myShootTimer = myShootTime;
		return true;
	}
	return false;
}

void Pistol::Reload()
{
	myAmmoInClip = myClipSize;
}

void Pistol::Update(float aDelta)
{
	myShootTimer -= aDelta;
}

void Pistol::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition)
{
	if (aComponent != nullptr)
	{
		if (aComponent->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			aComponent->AddForce(aDirection, 25.f);
		}
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Shotgun", aHitPosition));
		//aComponent->GetEntity().SendNote<DamageNote>(DamageNote(myDamage));

		PostMaster::GetInstance()->SendMessage(NetworkOnHitMessage(myDamage, aComponent->GetEntity().GetNetworkID()));
	}
}