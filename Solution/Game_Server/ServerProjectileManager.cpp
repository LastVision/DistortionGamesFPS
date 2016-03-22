#include "stdafx.h"
#include "ServerProjectileManager.h"
#include <GameEnum.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <DL_Debug.h>

ServerProjectileManager* ServerProjectileManager::myInstance = nullptr;

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
	return myInstance;
}

void ServerProjectileManager::CreateBullets(Prism::Scene* aScene)
{
	for (int i = 0; i < myBullets.GetCapacity(); ++i)
	{
		Entity* bullet = EntityFactory::CreateEntity((50000 + i), eEntityType::BULLET, aScene, false, CU::Vector3<float>(0, -10.f, 0));
		bullet->Kill();
		myBullets.Add(bullet);
	}
}
