#include "stdafx.h"
#include <AudioInterface.h>
#include "ClientProjectileManager.h"
#include <EntityFactory.h>
#include <PhysicsComponent.h>
#include <SoundComponent.h>

ClientProjectileManager::ClientProjectileManager()
{
}


ClientProjectileManager::~ClientProjectileManager()
{
	myBullets.DeleteAll();
	myExplosions.DeleteAll();
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

void ClientProjectileManager::Update(float aDeltaTime)
{
	__super::Update(aDeltaTime);
	for (int i = myLiveExplosions.Size() - 1; i >= 0; --i)
	{
		if (myLiveExplosions[i].myFrameCount >= 2)
		{
			myLiveExplosions[i].myExplosion->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveExplosions.RemoveCyclicAtIndex(i);
			continue;
		}
		++myLiveExplosions[i].myFrameCount;
	}
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

void ClientProjectileManager::CreateGrenades(Prism::Scene* aScene)
{
	for (int i = 0; i < myGrenades.GetCapacity(); ++i)
	{
		Entity* grenade = EntityFactory::CreateEntity((60000 + i + (i * 1)), eEntityType::GRENADE, "local", aScene, true, CU::Vector3<float>(0, -10.f, 0));
		grenade->Kill();
		myGrenades.Add(grenade);
	}
}

void ClientProjectileManager::RequestExplosion(const CU::Vector3<float>& aPosition, unsigned int aGID)
{
	bool explosionFound = false;
	for (int i = 0; i < myExplosions.Size(); ++i)
	{
		if (myExplosions[i]->GetGID() == aGID)
		{
			myLiveExplosions.Add(ExplosionStruct(myExplosions[i]));
			myLiveExplosions.GetLast().myExplosion->GetComponent<PhysicsComponent>()->AddToScene();
			myLiveExplosions.GetLast().myExplosion->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition);
			explosionFound = true;
		}
	}
	DL_ASSERT_EXP(explosionFound == true, "EXPLOSION NOT FOUND");
}

void ClientProjectileManager::CreateExplosions()
{
	myExplosions.Init(myGrenades.GetCapacity());
	myLiveExplosions.Init(myExplosions.GetCapacity());
	for (int i = 0; i < myGrenades.GetCapacity(); ++i)
	{
		Entity* explosion = EntityFactory::CreateEntity(myGrenades[i]->GetGID() + 1, eEntityType::EXPLOSION, nullptr, true, CU::Vector3<float>(0, -10.f, 0));
		myExplosions.Add(explosion);
	}
}

void ClientProjectileManager::KillGrenade(unsigned int aGID)
{
	for (int i = myLiveGrenades.Size() - 1; i >= 0; --i)
	{
		if (myLiveGrenades[i]->GetGID() == aGID)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_GrenadeExplosion"
				, myLiveGrenades[i]->GetComponent<SoundComponent>()->GetAudioSFXID());

			myLiveGrenades[i]->RemoveFromScene();
			myLiveGrenades[i]->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveGrenades.RemoveCyclicAtIndex(i);
			continue;
		}
	}
}