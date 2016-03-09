#include "stdafx.h"

#include "DamageNote.h"
#include "Entity.h"
#include "EntityFactory.h"
#include "GrenadeLauncher.h"
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include <XMLReader.h>


GrenadeLauncher::GrenadeLauncher(Prism::Scene* aScene)
	: Weapon(eWeaponType::GRENADE_LAUNCHER)
	, myScene(aScene)
	, myBullets(1024)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	tinyxml2::XMLElement* grenadeLauncherElement = reader.ForceFindFirstChild(root, "grenadelauncher");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "clipsize"), "value", myClipSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "damage"), "value", myDamage);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(grenadeLauncherElement, "startammo"), "value", myAmmoTotal);
	myAmmoInClip = myClipSize;

	reader.CloseDocument();
}


GrenadeLauncher::~GrenadeLauncher()
{
	myBullets.DeleteAll();
}


void GrenadeLauncher::Shoot(const CU::Matrix44<float>& aOrientation)
{
	aOrientation;
	if (myAmmoInClip > 0 && myBullets.Size() < 1024)
	{
		ShootAtDirection(aOrientation);
		myAmmoInClip -= 1;
	}
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
		myBullets[i]->Update(aDelta);
	}
}

void GrenadeLauncher::ShootAtDirection(const CU::Matrix44<float>& aOrientation)
{
	//bool newingInRunTimeHere = true;
	SET_RUNTIME(false);
	Entity* bullet = EntityFactory::CreateEntity(eEntityType::PROJECTILE, myScene, true, aOrientation.GetPos());
	RESET_RUNTIME;
	bullet->Reset();
	bullet->AddToScene();
	//bullet->GetPhysEntity()->SetPosition(aOrientation.GetPos());
	bullet->GetPhysEntity()->AddForce(aOrientation.GetForward(), 20.f);

	myBullets.Add(bullet);
}