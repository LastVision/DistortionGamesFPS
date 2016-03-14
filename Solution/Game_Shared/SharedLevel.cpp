#include "stdafx.h"
#include <CollisionNote.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include "SharedLevel.h"
#include <TriggerComponent.h>
#include <UpgradeComponent.h>
#include <UpgradeNote.h>

SharedLevel::SharedLevel()
	: myActiveEntities(256)
	, myActiveEnemies(512)
	, myInactiveEntities(64)
	, myInactiveEnemies(64)
	, myPlayers(16)
{
	Prism::PhysicsInterface::Create(std::bind(&SharedLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2));
}

SharedLevel::~SharedLevel()
{
	myPlayers.DeleteAll();
	myActiveEnemies.DeleteAll();
	myActiveEntities.DeleteAll();
	myInactiveEntities.DeleteAll();
	myInactiveEnemies.DeleteAll();
	Prism::PhysicsInterface::Destroy();
}

void SharedLevel::AddEntity(Entity* aEntity)
{
	myActiveEntities.Add(aEntity);
}

void SharedLevel::AddEnemy(Entity* anEntity)
{
	myActiveEnemies.Add(anEntity);
}

void SharedLevel::Update(const float aDeltaTime)
{
	for (Entity* entity : myPlayers)
	{
		if (entity->IsAlive() == true)
		{
			entity->Update(aDeltaTime);
		}
	}
	for (Entity* entity : myActiveEnemies)
	{
		if (entity->IsAlive() == true)
		{
			entity->Update(aDeltaTime);
		}
	}
	for (Entity* entity : myActiveEntities)
	{
		if (entity->IsAlive() == true)
		{
			entity->Update(aDeltaTime);
		}
	}
	CleanUp();
}

void SharedLevel::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond)
{
	if (aSecond->GetEntity().GetType() == eEntityType::PLAYER)
	{
		TriggerComponent* firstTrigger = aFirst->GetEntity().GetComponent<TriggerComponent>();

		if (firstTrigger->GetTriggerType() == eTriggerType::UPGRADE)
		{
			aSecond->GetEntity().SendNote<UpgradeNote>(aFirst->GetEntity().GetComponent<UpgradeComponent>()->GetData());
		}
		else if (firstTrigger->GetTriggerType() == eTriggerType::UNLOCK)
		{
			//firstTrigger->
		}
		else
		{
			aSecond->GetEntity().SendNote<CollisionNote>(CollisionNote(&aFirst->GetEntity()));
		}
		aFirst->GetEntity().SendNote<CollisionNote>(CollisionNote(&aSecond->GetEntity()));
	}


}

void SharedLevel::CleanUp()
{
	for (int i = myActiveEnemies.Size() - 1; i >= 0; --i)
	{
		if (myActiveEnemies[i]->IsAlive() == false)
		{
			myInactiveEnemies.Add(myActiveEnemies[i]);
			myActiveEnemies.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myActiveEntities.Size() - 1; i >= 0; --i)
	{
		if (myActiveEntities[i]->IsAlive() == false)
		{
			myInactiveEntities.Add(myActiveEntities[i]);
			myActiveEntities.RemoveCyclicAtIndex(i);
		}
	}
}