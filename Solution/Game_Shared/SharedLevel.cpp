#include "stdafx.h"
#include <CollisionNote.h>
#include <DamageNote.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <EmitterMessage.h>
#include <NetMessageSetActive.h>
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include "SharedLevel.h"
#include <SharedNetworkManager.h>
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

	switch (first.GetType())
	{
	case eEntityType::TRIGGER:
		HandleTrigger(first, second);
		break;
	case eEntityType::EXPLOSION:
		HandleExplosion(first, second);
		break;
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

void SharedLevel::HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity)
{
	TriggerComponent* firstTrigger = aFirstEntity.GetComponent<TriggerComponent>();
	if (firstTrigger->GetTriggerType() == eTriggerType::UPGRADE && firstTrigger->GetTriggerType() == eTriggerType::HEALTH_PACK) //clientside
	{
		if (aSecondEntity.GetType() == eEntityType::PLAYER)
		{
			aSecondEntity.SendNote<UpgradeNote>(aFirstEntity.GetComponent<UpgradeComponent>()->GetData());
		}
	}
	else //serverside
	{
		if (aSecondEntity.GetType() == eEntityType::UNIT && aSecondEntity.GetSubType() == "player")
		{
			switch (firstTrigger->GetTriggerType())
			{
			case eTriggerType::UNLOCK:
				//PostMaster::GetInstance()->SendMessage(EmitterMessage("Unlock", myActiveEntitiesMap[firstTrigger->GetValue()]->GetOrientation().GetPos()));
				SharedNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(false, firstTrigger->GetValue()));
				myActiveEntitiesMap[firstTrigger->GetValue()]->GetComponent<PhysicsComponent>()->RemoveFromScene();
				//myActiveEntitiesMap[firstTrigger->GetValue()]->RemoveFromScene();
				// do "open" animation
				break;
			case eTriggerType::LOCK:
				SharedNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(true, firstTrigger->GetValue()));
				myActiveEntitiesMap[firstTrigger->GetValue()]->GetComponent<PhysicsComponent>()->AddToScene();
				//myActiveEntitiesMap[firstTrigger->GetValue()]->AddToScene();
				// do "close" animation
				break;
			}
		}
	}
	aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity));
	aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity));


	/*
	if (aSecondEntity.GetType() == eEntityType::PLAYER)
	{
		TriggerComponent* firstTrigger = aFirstEntity.GetComponent<TriggerComponent>();

		if (firstTrigger != nullptr)
		{
			switch (firstTrigger->GetTriggerType())
			{
			case eTriggerType::UPGRADE:
				aSecondEntity.SendNote<UpgradeNote>(aFirstEntity.GetComponent<UpgradeComponent>()->GetData());
				break;
			case eTriggerType::UNLOCK:
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Unlock", myActiveEntitiesMap[firstTrigger->GetValue()]->GetOrientation().GetPos()));
				myActiveEntitiesMap[firstTrigger->GetValue()]->GetComponent<PhysicsComponent>()->RemoveFromScene();
				myActiveEntitiesMap[firstTrigger->GetValue()]->RemoveFromScene();
				// do "open" animation
				break;
			case eTriggerType::LOCK:
				myActiveEntitiesMap[firstTrigger->GetValue()]->GetComponent<PhysicsComponent>()->AddToScene();
				myActiveEntitiesMap[firstTrigger->GetValue()]->AddToScene();
				// do "close" animation
				break;
			default:
				aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity));
				break;
			}
			aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity));
		}
	}
	*/
}

void SharedLevel::HandleExplosion(Entity& aFirstEntity, Entity& aSecondEntity)
{
	PhysicsComponent* physicsComponent = aSecondEntity.GetComponent<PhysicsComponent>();

	if (physicsComponent->GetPhysicsType() == ePhysics::DYNAMIC)
	{
		aFirstEntity.GetComponent<PhysicsComponent>()->UpdateOrientationStatic();
		CU::Vector3<float> firstPos;
		memcpy(&firstPos, &aFirstEntity.GetComponent<PhysicsComponent>()->GetPosition()[0], sizeof(float) * 3);

		CU::Vector3<float> direction = CU::GetNormalized(aSecondEntity.GetOrientation().GetPos() - firstPos);
		physicsComponent->AddForce(direction, 5.f);
	}
	else if (physicsComponent->GetPhysicsType() == ePhysics::CAPSULE)
	{
		aSecondEntity.SendNote<DamageNote>(DamageNote(10));
	}
}