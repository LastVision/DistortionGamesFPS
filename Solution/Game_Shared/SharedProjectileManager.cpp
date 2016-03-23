#include "stdafx.h"
#include "SharedProjectileManager.h"
#include <Entity.h>

SharedProjectileManager* SharedProjectileManager::myInstance = nullptr;

SharedProjectileManager::SharedProjectileManager()
	: myBullets(64)
	, myLiveBullets(64)
	, myGrenades(128)
	, myLiveGrenades(128)
	, myBulletIndex(0)
	, myGrenadeIndex(0)
{
}

SharedProjectileManager::~SharedProjectileManager()
{
	myLiveBullets.RemoveAll();
	myBullets.DeleteAll();
	myGrenades.DeleteAll();
	myLiveGrenades.RemoveAll();
}

SharedProjectileManager* SharedProjectileManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ServerProjectileManager were null!");
	return myInstance;
}

int SharedProjectileManager::GetBulletIndex()
{
	return myBulletIndex;
}

Entity* SharedProjectileManager::RequestBullet()
{
	if (myBulletIndex >= myBullets.Size())
	{
		myBulletIndex = 0;
	}

	Entity* toReturn = myBullets[myBulletIndex];
	myBulletIndex++;
	myLiveBullets.Add(toReturn);
	return toReturn;
}

Entity* SharedProjectileManager::RequestBullet(unsigned int aGID)
{
	for each (Entity* bullet in myBullets)
	{
		if (bullet->GetGID() == aGID)
		{
			myLiveBullets.Add(bullet);
			return bullet;
		}
	}
	return nullptr;
}

void SharedProjectileManager::ActivateBullet(Entity* anEntity)
{
	anEntity->Reset();
}

void SharedProjectileManager::Update(float aDeltaTime)
{
	for (int i = myLiveBullets.Size() - 1; i >= 0; --i)
	{
		if (myLiveBullets[i]->IsAlive() == true)
		{
			myLiveBullets[i]->Update(aDeltaTime);
		}
		else
		{
			myLiveBullets.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myLiveGrenades.Size() - 1; i >= 0; --i)
	{
		if (myLiveGrenades[i]->IsAlive() == true)
		{
			myLiveGrenades[i]->Update(aDeltaTime);
		}
		else
		{
			myLiveGrenades.RemoveCyclicAtIndex(i);
		}
	}
}

Entity* SharedProjectileManager::RequestGrenade()
{
	if (myGrenadeIndex >= myGrenades.Size())
	{
		myGrenadeIndex = 0;
	}

	Entity* toReturn = myGrenades[myGrenadeIndex];
	++myGrenadeIndex;
	myLiveGrenades.Add(toReturn);
	return toReturn;
}
