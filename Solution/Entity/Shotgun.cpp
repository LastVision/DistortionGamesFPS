#include "stdafx.h"

#include <AudioInterface.h>
#include "DamageNote.h"
#include "Entity.h"
#include <EmitterMessage.h>
#include <NetMessageOnHit.h>
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include "PhysicsComponent.h"
#include "Shotgun.h"
#include <SharedNetworkManager.h>
#include <XMLReader.h>

Shotgun::Shotgun(Entity* aOwnerEntity)
	: Weapon(eWeaponType::SHOTGUN, "shotgun", aOwnerEntity)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* shotgunElement = reader.ForceFindFirstChild(root, "shotgun");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "startammo"), "value", myAmmoTotal);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "minspreadrotation"), "value", myMinSpreadRotation);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "maxspreadrotation"), "value", myMaxSpreadRotation);
	
	reader.CloseDocument();

	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition);
	};
}


Shotgun::~Shotgun()
{
}


bool Shotgun::Shoot(const CU::Matrix44<float>& aOrientation)
{
	if (myAmmoInClip > 0 && myShootTimer <= 0.f)
	{
		ShootRowAround(aOrientation, CU::Vector3<float>(0, 0, 1.f) * (CU::Matrix44<float>::CreateRotateAroundX(CU::Math::RandomRange(-myMaxSpreadRotation, -myMinSpreadRotation)) * aOrientation));
		ShootRowAround(aOrientation, aOrientation.GetForward());
		ShootRowAround(aOrientation, CU::Vector3<float>(0, 0, 1.f) * (CU::Matrix44<float>::CreateRotateAroundX(CU::Math::RandomRange(myMinSpreadRotation, myMaxSpreadRotation)) * aOrientation));
		myAmmoInClip -= 1;
		myShootTimer = myShootTime;
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Shotgun", 0);
		return true;
	}
	return false;
}

void Shotgun::Reload()
{
	int ammoLeft = myAmmoTotal;
	myAmmoTotal -= min(ammoLeft, myClipSize - myAmmoInClip);
	myAmmoInClip = min(myClipSize, myAmmoTotal);
}

void Shotgun::Update(float aDelta)
{
	myShootTimer -= aDelta;
}

void Shotgun::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition)
{
	if (aComponent != nullptr)
	{
		if (aComponent->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			aComponent->AddForce(aDirection, myForceStrength);
		}
		//aComponent->GetEntity().SendNote<DamageNote>(DamageNote(myDamage));
		SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(myDamage, aComponent->GetEntity().GetGID()));
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Shotgun", aHitPosition));
	}
}

void Shotgun::Upgrade(const UpgradeComponentData& aData)
{
	Weapon::Upgrade(aData);
	myMinSpreadRotation += aData.myMinSpreadRotation;
	myMaxSpreadRotation += aData.myMaxSpreadRotation;
}

void Shotgun::ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward)
{
	CU::Vector3<float> forward = aForward;

	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos(), forward, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());

	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
		, forward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(-myMaxSpreadRotation, -myMinSpreadRotation))
		, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());

	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
		, forward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(myMinSpreadRotation, myMaxSpreadRotation))
		, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());
}
