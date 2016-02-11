#pragma once

class Entity;

class EntityId
{
public:
	static EntityId* GetInstance();
	static void Destroy();

	int GetId(Entity* anEntity);
	Entity* GetEntity(int aId) const;
	Entity* GetTrigger(eEntityType aType, int aId) const;
	
private:
	EntityId();
	~EntityId();

	CU::GrowingArray<Entity*> myEntities;
	CU::GrowingArray<Entity*> myResourcePoints;
	CU::GrowingArray<Entity*> myVictoryPoints;

	static EntityId* myInstance;
};

