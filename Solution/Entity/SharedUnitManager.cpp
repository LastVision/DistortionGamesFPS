#include "stdafx.h"
#include "SharedUnitManager.h"
#include "Entity.h"
#include <Utility.h>


SharedUnitManager* SharedUnitManager::myInstance = nullptr;

SharedUnitManager::SharedUnitManager()
	: myUnits(512)
	, myActiveUnits(128)
{
}


SharedUnitManager::~SharedUnitManager()
{
	myActiveUnits.RemoveAll();
	myUnits.DeleteAll();
}

SharedUnitManager* SharedUnitManager::GetInstance()
{
	return myInstance;
}

void SharedUnitManager::Update(float aDeltaTime)
{
	for (int i = myActiveUnits.Size() - 1; i >= 0; --i)
	{
		if (myActiveUnits[i]->IsAlive() == true)
		{
			myActiveUnits[i]->Update(aDeltaTime);
		}
		else
		{
			myActiveUnits.RemoveCyclicAtIndex(i);
		}
	}
}

void SharedUnitManager::ActivateUnit(Entity* aUnit)
{
	aUnit->Reset();
	myActiveUnits.Add(aUnit);
}

Entity* SharedUnitManager::RequestUnit(const std::string& aUnitType)
{
	if (myActiveUnits.Size() < myActiveUnits.GetCapacity())
	{
		if (myUnitIndex >= myUnits.Size())
		{
			myUnitIndex = 0;
		}

		if (myUnits[myUnitIndex]->GetSubType() == aUnitType && myUnits[myUnitIndex]->IsAlive() == false)
		{
			Entity* toReturn = myUnits[myUnitIndex];
			myUnitIndex++;
			return toReturn;
		}
		else
		{
			return SearchForUnit(aUnitType);
		}
	}
	return nullptr;
}


Entity* SharedUnitManager::SearchForUnit(const std::string& aUnitType)
{
	for (int i = myUnitIndex; i < myUnits.Size(); ++i)
	{
		if (myUnits[i]->GetSubType() == aUnitType && myUnits[i]->IsAlive() == false)
		{
			return myUnits[i];
		}
		myUnitIndex++;
	}
}

Entity* SharedUnitManager::RequestUnit(unsigned int aGID)
{
	for each (Entity* unit in myUnits)
	{
		if (unit->GetGID() == aGID)
		{
			return unit;
		}
	}
	return nullptr;
}
