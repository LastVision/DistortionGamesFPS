#include "stdafx.h"
#include <CollisionNote.h>
#include <DamageNote.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <EmitterMessage.h>
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include "SharedLevel.h"
#include <TriggerComponent.h>
#include <UpgradeComponent.h>
#include <UpgradeNote.h>

#include <ProjectileComponent.h>

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
	myActiveEntitiesMap[aEntity->GetGID()] = aEntity;
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
	Entity& first = aFirst->GetEntity();
	Entity& second = aSecond->GetEntity();

	if (first.GetType() == eEntityType::TRIGGER)
	{
		if (second.GetType() == eEntityType::PLAYER)
		{
			TriggerComponent* firstTrigger = first.GetComponent<TriggerComponent>();

			if (firstTrigger != nullptr)
			{
				if (firstTrigger->GetTriggerType() == eTriggerType::UPGRADE)
				{
					second.SendNote<UpgradeNote>(first.GetComponent<UpgradeComponent>()->GetData());
				}
				else if (firstTrigger->GetTriggerType() == eTriggerType::UNLOCK)
				{
					PostMaster::GetInstance()->SendMessage(EmitterMessage("Unlock", myActiveEntitiesMap[firstTrigger->GetValue()]->GetOrientation().GetPos()));
					myActiveEntitiesMap[firstTrigger->GetValue()]->Kill();
				}
				else
				{
					second.SendNote<CollisionNote>(CollisionNote(&first));
				}
				first.SendNote<CollisionNote>(CollisionNote(&second));
			}
		}
	}
	else if (first.GetType() == eEntityType::EXPLOSION)
	{
		PhysicsComponent* physicsComponent = second.GetComponent<PhysicsComponent>();
	
		if (physicsComponent->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			CU::Vector3<float> direction = second.GetOrientation().GetPos() - first.GetOrientation().GetPos();
			physicsComponent->AddForce(direction, 5.f);
		}
		else if (physicsComponent->GetPhysicsType() == ePhysics::CAPSULE)
		{
			second.SendNote<DamageNote>(DamageNote(10));
		}

		//first.GetComponent<PhysicsComponent>()->RemoveFromScene();
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