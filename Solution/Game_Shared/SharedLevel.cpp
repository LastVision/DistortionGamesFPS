#include "stdafx.h"
#include <CollisionNote.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include "SharedLevel.h"

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
		entity->Update(aDeltaTime);
	}
	for (Entity* entity : myEnemies)
	{
		entity->Update(aDeltaTime);
	}
	for (Entity* entity : myEntities)
	{
		entity->Update(aDeltaTime);
	}
}

void SharedLevel::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond)
{
	aFirst->GetEntity().SendNote<CollisionNote>(CollisionNote(&aSecond->GetEntity()));
	aSecond->GetEntity().SendNote<CollisionNote>(CollisionNote(&aFirst->GetEntity()));
}