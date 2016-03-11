#include "stdafx.h"
#include <CollisionNote.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include "SharedLevel.h"
#include <UpgradeComponent.h>
#include <UpgradeNote.h>

SharedLevel::SharedLevel()
	: myEntities(256)
	, myEnemies(512)
	, myPlayers(16)
{
	Prism::PhysicsInterface::Create(std::bind(&SharedLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2));
}

SharedLevel::~SharedLevel()
{
	myPlayers.DeleteAll();
	myEnemies.DeleteAll();
	myEntities.DeleteAll();
	Prism::PhysicsInterface::Destroy();
}

void SharedLevel::AddEntity(Entity* aEntity)
{
	myEntities.Add(aEntity);
}

void SharedLevel::AddEnemy(Entity* anEntity)
{
	myEnemies.Add(anEntity);
}

void SharedLevel::Update(const float aDeltaTime)
{
	for (Entity* entity : myPlayers)
	{
		//if (entity->IsAlive() == true)
		{
			entity->Update(aDeltaTime);
		}
	}
	for (Entity* entity : myEnemies)
	{
		//if (entity->IsAlive() == true)
		{
			entity->Update(aDeltaTime);
		}
	}
	for (Entity* entity : myEntities)
	{
		//if (entity->IsAlive() == true)
		{
			entity->Update(aDeltaTime);
		}
	}
}

void SharedLevel::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond)
{
	if (aSecond->GetEntity().GetType() == eEntityType::PLAYER)
	{
		if (aFirst->GetEntity().GetComponent<UpgradeComponent>() != nullptr)
		{
			aSecond->GetEntity().SendNote<UpgradeNote>(aFirst->GetEntity().GetComponent<UpgradeComponent>()->GetData());
		}
		else
		{
			aSecond->GetEntity().SendNote<CollisionNote>(CollisionNote(&aFirst->GetEntity()));
		}
		aFirst->GetEntity().SendNote<CollisionNote>(CollisionNote(&aSecond->GetEntity()));
	}


}