#include "stdafx.h"

#include "EntityId.h"
#include "PollingStation.h"

EntityId* EntityId::myInstance = nullptr;

EntityId* EntityId::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new EntityId();
	}

	return myInstance;
}

void EntityId::Destroy()
{
	SAFE_DELETE(myInstance);
}

int EntityId::GetId(Entity* anEntity)
{
	//DL_ASSERT_EXP(////Prism::MemoryTracker::GetInstance()->GetRunTime() == false
	//	, "Don't call EntityId in runtime, too expensive.");
	int index = myEntities.Find(anEntity);
	if (index < 0)
	{
		myEntities.Add(anEntity);
		index = myEntities.Size() - 1;

		if (anEntity->GetType() == eEntityType::RESOURCE_POINT)
		{
			myResourcePoints.Add(anEntity);
			PollingStation::GetInstance()->RegisterEntity(anEntity);
		}

		if (anEntity->GetType() == eEntityType::VICTORY_POINT)
		{
			myVictoryPoints.Add(anEntity);
			PollingStation::GetInstance()->RegisterEntity(anEntity);
		}
	}
	return index;
}

Entity* EntityId::GetEntity(int aId) const
{
	if (aId >= myEntities.Size() || aId < 0)
	{
		return nullptr;
	}
	return myEntities[aId];
}

Entity* EntityId::GetTrigger(eEntityType aType, int aId) const
{
	switch (aType)
	{
	case RESOURCE_POINT:
		if (aId >= myResourcePoints.Size() || aId < 0)
		{
			return nullptr;
		}
		return myResourcePoints[aId];
		break;
	case VICTORY_POINT:
		if (aId >= myVictoryPoints.Size() || aId < 0)
		{
			return nullptr;
		}
		return myVictoryPoints[aId];
		break;
	default:
		DL_ASSERT("Trigger not found.");
		break;
	}
	return nullptr;
}

EntityId::EntityId()
	: myEntities(4096)
	, myResourcePoints(GC::resourcePointCount)
	, myVictoryPoints(GC::victoryPointCount)
{
}


EntityId::~EntityId()
{
}
