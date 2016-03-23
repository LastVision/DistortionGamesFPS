#include "stdafx.h"
#include "ServerProjectileManager.h"
#include <GameEnum.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <DL_Debug.h>

ServerProjectileManager::ServerProjectileManager()
{
}

ServerProjectileManager::~ServerProjectileManager()
{
	myBullets.DeleteAll();
}

void ServerProjectileManager::Create()
{
	SET_RUNTIME(false);
	DL_ASSERT_EXP(myInstance == nullptr, "Tried to create ServerProjectileManager while it already existed!");
	myInstance = new ServerProjectileManager();
}

void ServerProjectileManager::Destroy()
{
	DL_ASSERT_EXP(myInstance != nullptr, "Tried to create ServerProjectileManager while it already existed!");
	delete myInstance;
	myInstance = nullptr;
}

ServerProjectileManager* ServerProjectileManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ServerProjectileManager were null!");
	return static_cast<ServerProjectileManager*>(myInstance);
}

void ServerProjectileManager::CreateBullets(Prism::Scene* aScene)
{
	for (int i = 0; i < myBullets.GetCapacity(); ++i)
	{
		Entity* bullet = EntityFactory::CreateEntity((50000 + i), eEntityType::BULLET, "bulletserver", aScene, false, CU::Vector3<float>(0, -10.f, 0));
		bullet->Kill();
		myBullets.Add(bullet);
	}
}

void ServerProjectileManager::CreateGrenades(Prism::Scene* aScene)
{
	for (int i = 0; i < myGrenades.GetCapacity(); ++i)
	{
		Entity* grenade = EntityFactory::CreateEntity((60000 + i), eEntityType::GRENADE, "server", aScene, false, CU::Vector3<float>(0, -10.f, 0));
		grenade->Kill();
		myGrenades.Add(grenade);
	}
}
