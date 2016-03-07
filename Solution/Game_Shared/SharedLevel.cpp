#include "stdafx.h"
#include "SharedLevel.h"
#include <Entity.h>
#include <EntityFactory.h>

SharedLevel::SharedLevel()
	: myEntities(256)
	, myEnemies(512)
	, myPlayers(16)
{
}

SharedLevel::~SharedLevel()
{
	myEntities.DeleteAll();
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
	for (Entity* entity : myEntities)
	{
		entity->Update(aDeltaTime);
	}

	for (Entity* entity : myEnemies)
	{
		entity->Update(aDeltaTime);
	}

}
