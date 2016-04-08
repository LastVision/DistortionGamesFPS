#include "stdafx.h"

#include <AudioInterface.h>
#include "DamageNote.h"
#include "Entity.h"
#include <EmitterMessage.h>
#include <NetMessageOnHit.h>
#include <NetMessageRayCastRequest.h>
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include "PhysicsComponent.h"
#include "Shotgun.h"
#include <SharedNetworkManager.h>
#include "SoundComponent.h"
#include <XMLReader.h>

Shotgun::Shotgun(Entity* aOwnerEntity)
	: Weapon(eWeaponType::SHOTGUN, aOwnerEntity)
{
	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition, aHitNormal);
	};
}

Shotgun::~Shotgun()
{
}

void Shotgun::Init(std::string aWeaponSettingsPath, std::string aXMLTagName)
{
	Weapon::Init(aWeaponSettingsPath, aXMLTagName);

	XMLReader reader;
	reader.OpenDocument(aWeaponSettingsPath);
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* shotgunElement = reader.ForceFindFirstChild(root, "shotgun");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "startammo"), "value", myAmmoTotal);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "minspreadrotation"), "value", myMinSpreadRotation);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "maxspreadrotation"), "value", myMaxSpreadRotation);

	reader.CloseDocument();
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

void Shotgun::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (aComponent != nullptr)
	{
		if (aComponent->GetEntity().GetComponent<SoundComponent>() != nullptr)
		{
			if (aComponent->GetEntity().GetType() == eEntityType::UNIT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EnemyTakeDamage", aComponent->GetEntity().GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
#ifdef RELEASE_BUILD
		if (aComponent->GetEntity().GetSubType() != "player")
#endif
		{
			if (aComponent->GetPhysicsType() == ePhysics::DYNAMIC)
			{
				aComponent->AddForce(aDirection, myForceStrength);
			}
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(myDamage, aComponent->GetEntity().GetGID()));
			CU::Vector3<float> toSend = CU::Reflect(aDirection, aHitNormal);

			if (aComponent->GetEntity().GetIsEnemy() == true)
			{
				PostMaster::GetInstance()->SendMessage(EmitterMessage("OnHit", aHitPosition, toSend));
			}
			else
			{
				PostMaster::GetInstance()->SendMessage(EmitterMessage("OnEnvHit", aHitPosition, aHitNormal));
			}
		}
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
	CU::Vector3<float> forward1 = aForward;
	CU::Vector3<float> forward2 = aForward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(-myMaxSpreadRotation, -myMinSpreadRotation));
	CU::Vector3<float> forward3 = aForward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(myMinSpreadRotation, myMaxSpreadRotation));

	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos(), forward1, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos(), forward2, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos(), forward3, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());

	SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(aOrientation.GetPos(), forward1, int(eNetRayCastType::CLIENT_SHOOT_SHOTGUN), 500.f, myOwnerEntity->GetGID()));
	SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(aOrientation.GetPos(), forward2, int(eNetRayCastType::CLIENT_SHOOT_SHOTGUN), 500.f, myOwnerEntity->GetGID()));
	SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(aOrientation.GetPos(), forward3, int(eNetRayCastType::CLIENT_SHOOT_SHOTGUN), 500.f, myOwnerEntity->GetGID()));
}
