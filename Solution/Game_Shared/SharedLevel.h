#pragma once
#include <GrowingArray.h>

class Entity;

class SharedLevel
{
public:
	SharedLevel();
	virtual ~SharedLevel();

	void AddEntity(Entity* aEntity);

	virtual void Update(const float aDeltaTime);

protected:
	CU::GrowingArray<Entity*> myEntities;
};

