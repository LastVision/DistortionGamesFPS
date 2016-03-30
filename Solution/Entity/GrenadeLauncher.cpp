#include "stdafx.h"

#include "DamageNote.h"
#include "EntityFactory.h"
#include "GrenadeLauncher.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include "GrenadeComponent.h"
#include <NetMessageShootGrenade.h>
#include <SharedNetworkManager.h>

GrenadeLauncher::GrenadeLauncher(Prism::Scene* aScene, unsigned int aEntityGID, Entity* aOwnerEntity)
	: Weapon(eWeaponType::GRENADE_LAUNCHER, "grenadelauncher", aOwnerEntity)
	, myScene(aScene)
	, myBullets(16)
	, myCurrentBulletToUse(0)
	, myEntityGID(aEntityGID)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* grenadeLauncherElement = reader.ForceFindFirstChild(root, "grenadelauncher");
	
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "startammo"), "value", myAmmoTotal);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "maxAmountOfGrenades"), "value", myMaxGrenades);

	reader.CloseDocument();


	for (int i = 0; i < myMaxGrenades; ++i)
	{
		Entity* bullet = EntityFactory::CreateEntity((60000 + i), eEntityType::GRENADE, myScene, true, CU::Vector3<float>());
		myBullets.Add(bullet);
		bullet->GetComponent<PhysicsComponent>()->Sleep();
	}
}

GrenadeLauncher::~GrenadeLauncher()
{
	myBullets.DeleteAll();
}

bool GrenadeLauncher::Shoot(const CU::Matrix44<float>&)
{
	if (myAmmoInClip > 0 && myShootTimer <= 0.f && myBullets.Size() < 1024)
	{
		//Skicka grenademessage(aOrientation.GetForward());
		SharedNetworkManager::GetInstance()->AddMessage<NetMessageShootGrenade>(NetMessageShootGrenade(int(myForceStrength)));

		//ShootAtDirection(aOrientation);
		myAmmoInClip -= 1;
		myShootTimer = myShootTime;
		return true;
	}
	return false;
}
void GrenadeLauncher::Reload()
{
	int ammoLeft = myAmmoTotal;
	myAmmoTotal -= min(ammoLeft, myClipSize - myAmmoInClip);
	myAmmoInClip = min(myClipSize, myAmmoTotal);
}

void GrenadeLauncher::Update(float aDelta)
{
	for (int i = 0; i < myBullets.Size(); ++i)
	{
		if (myBullets[i]->GetComponent<GrenadeComponent>()->GetShouldBeUpdated() == true)
		{
			myBullets[i]->Update(aDelta);
		}
	}
	myShootTimer -= aDelta;
}

void GrenadeLauncher::ShootAtDirection(const CU::Matrix44<float>& aOrientation)
{
	myBullets[myCurrentBulletToUse]->Reset();
	myBullets[myCurrentBulletToUse]->GetComponent<PhysicsComponent>()->TeleportToPosition(aOrientation.GetPos());
	myBullets[myCurrentBulletToUse]->GetComponent<GrenadeComponent>()->Activate(myEntityGID);
	myBullets[myCurrentBulletToUse]->GetComponent<PhysicsComponent>()->AddForce(aOrientation.GetForward(), myForceStrength);
	++myCurrentBulletToUse;

	if (myCurrentBulletToUse >= myMaxGrenades)
	{
		myCurrentBulletToUse = 0;
	}
}