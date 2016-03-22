#include "stdafx.h"
#include "ClientProjectileManager.h"
#include <EntityFactory.h>


ClientProjectileManager::ClientProjectileManager()
{
}


ClientProjectileManager::~ClientProjectileManager()
{
	myBullets.DeleteAll();
}

void ClientProjectileManager::Create()
{
	SET_RUNTIME(false);
	DL_ASSERT_EXP(myInstance == nullptr, "Tried to create ClientProjectileManager while it already existed!");
	myInstance = new ClientProjectileManager();
}

void ClientProjectileManager::Destroy()
{
	DL_ASSERT_EXP(myInstance != nullptr, "Tried to create ClientProjectileManager while it already existed!");
	delete myInstance;
	myInstance = nullptr;
}

ClientProjectileManager* ClientProjectileManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ClientProjectileManager were null!");
	return static_cast<ClientProjectileManager*>(myInstance);
}

void ClientProjectileManager::CreateBullets(Prism::Scene* aScene)
{
	for (int i = 0; i < myBullets.GetCapacity(); ++i)
	{
		Entity* bullet = EntityFactory::CreateEntity((50000 + i), eEntityType::BULLET, "bulletclient", aScene, true, CU::Vector3<float>(0, -10.f, 0));
		bullet->Kill();
		myBullets.Add(bullet);
	}
}
