#include "stdafx.h"
#include <CollisionNote.h>
#include <DamageNote.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <EmitterMessage.h>
#include <NetMessageSetActive.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include "SharedLevel.h"
#include <SharedNetworkManager.h>
#include <TriggerComponent.h>
#include <UpgradeComponent.h>
#include <UpgradeNote.h>

#include <GrenadeComponent.h>

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
	myActiveUnitsMap[anEntity->GetGID()] = anEntity;
}

void SharedLevel::AddPlayerStartPosition(unsigned int aGID, const CU::Vector3<float>& aPosition)
{
	myPlayerStartPositions[aGID] = aPosition;
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
	/*for (Entity* entity : myActiveEnemies)
	{
	if (entity->IsAlive() == true)
	{
	entity->Update(aDeltaTime);
	}
	}*/
	for (Entity* entity : myActiveEntities)
	{
		if (entity->IsAlive() == true)
		{
			entity->Update(aDeltaTime);
		}
	}
	CleanUp();
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