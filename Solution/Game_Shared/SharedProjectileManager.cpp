#include "stdafx.h"
#include "SharedProjectileManager.h"
#include <Entity.h>

SharedProjectileManager* SharedProjectileManager::myInstance = nullptr;

SharedProjectileManager::SharedProjectileManager()
	: myBullets(128)
	, myLiveBullets(128)
{
}

SharedProjectileManager::~SharedProjectileManager()
{
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
}
