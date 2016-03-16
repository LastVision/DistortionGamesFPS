#include "stdafx.h"

#include "DamageNote.h"
#include "Entity.h"
#include "EntityFactory.h"
#include "GrenadeLauncher.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include "ProjectileComponent.h"
#include <XMLReader.h>


GrenadeLauncher::GrenadeLauncher(Prism::Scene* aScene, unsigned int aEntityGID)
	: Weapon(eWeaponType::GRENADE_LAUNCHER)
	, myScene(aScene)
	, myBullets(16)
	, myCurrentBulletToUse(0)
	, myEntityGID(aEntityGID)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	tinyxml2::XMLElement* grenadeLauncherElement = reader.ForceFindFirstChild(root, "grenadelauncher");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "clipsize"), "value", myClipSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "damage"), "value", myDamage);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "startammo"), "value", myAmmoTotal);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "shoottime"), "value", myShootTime);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "forceStrength"), "value", myForceStrength);

	myAmmoInClip = myClipSize;
	myShootTimer = myShootTime;

	reader.CloseDocument();


	for (int i = 0; i < 8; ++i)
	{
		Entity* bullet = EntityFactory::CreateEntity((100000 + i), eEntityType::PROJECTILE, myScene, true, CU::Vector3<float>());
		myBullets.Add(bullet);
		bullet->GetComponent<PhysicsComponent>()->Sleep();
	}
}


GrenadeLauncher::~GrenadeLauncher()
{
	myBullets.DeleteAll();
}


bool GrenadeLauncher::Shoot(const CU::Matrix44<float>& aOrientation)
{
	aOrientation;
	if (myAmmoInClip > 0 && myShootTimer <= 0.f && myBullets.Size() < 1024)
	{
		ShootAtDirection(aOrientation);
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
	myAmmoInClip = myClipSize;
}

void GrenadeLauncher::Update(float aDelta)
{
	for (int i = 0; i < myBullets.Size(); ++i)
	{
		if (myBullets[i]->GetComponent<ProjectileComponent>()->GetShouldBeUpdated() == true)
		{
			myBullets[i]->Update(aDelta);
		}
	}
	myShootTimer -= aDelta;
}

void GrenadeLauncher::ShootAtDirection(const CU::Matrix44<float>& aOrientation)
{
	myBullets[myCurrentBulletToUse]->Reset();
	//if (myBullets[myCurrentBulletToUse]->IsInScene() == false)
	//{
	//	myBullets[myCurrentBulletToUse]->GetComponent<PhysicsComponent>()->AddToScene();
	//	myBullets[myCurrentBulletToUse]->AddToScene();
	//}

	myBullets[myCurrentBulletToUse]->GetComponent<ProjectileComponent>()->Activate(myEntityGID);
	myBullets[myCurrentBulletToUse]->GetComponent<PhysicsComponent>()->TeleportToPosition(aOrientation.GetPos());
	myBullets[myCurrentBulletToUse]->GetComponent<PhysicsComponent>()->AddForce(aOrientation.GetForward(), myForceStrength);
	++myCurrentBulletToUse;

	if (myCurrentBulletToUse > 7)
	{
		myCurrentBulletToUse = 0;
	}
	//myBullets.Add(bullet);
}