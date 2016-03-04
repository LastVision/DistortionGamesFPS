#include "stdafx.h"
#include "SharedLevel.h"
#include <Entity.h>

SharedLevel::SharedLevel()
	: myEntities(256)
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

void SharedLevel::Update(const float aDeltaTime)
{
	for (Entity* entity : myEntities)
	{
		entity->Update(aDeltaTime);
	}
}