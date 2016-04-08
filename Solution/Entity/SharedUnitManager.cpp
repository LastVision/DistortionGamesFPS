#include "stdafx.h"
#include "SharedUnitManager.h"
#include "Entity.h"
#include <Utility.h>
#include "PhysicsComponent.h"
#include <CommonHelper.h>

#include <PhysicsInterface.h>

SharedUnitManager* SharedUnitManager::myInstance = nullptr;

SharedUnitManager::SharedUnitManager()
	: myUnits(128)
	, myActiveUnits(128)
	, myUnitIndex(0)
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

void SharedUnitManager::ActivateUnit(Entity* aUnit, const CU::Vector3<float>& aPosition)
{
	aUnit->Reset();
	aUnit->SetActive(true);

	if (aUnit->GetIsClient() == false)
	{
		aUnit->GetComponent<PhysicsComponent>()->AddToScene();
		aUnit->GetComponent<PhysicsComponent>()->Wake();
		Prism::PhysicsInterface::GetInstance()->TeleportToPosition(aUnit->GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), aPosition);
	}
	else
	{
		aUnit->AddToScene();
		aUnit->GetComponent<PhysicsComponent>()->AddToScene();
	}
	
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

		if (myUnits[myUnitIndex]->GetSubType() == CU::ToLower(aUnitType) && myUnits[myUnitIndex]->IsAlive() == false)
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


void SharedUnitManager::AddToMap()
{
	for (unsigned int i = 0; i < myUnits.Size(); ++i)
	{
		myUnitsMap[myUnits[i]->GetGID()] = myUnits[i];
	}
}

Entity* SharedUnitManager::SearchForUnit(const std::string& aUnitType)
{
	myUnitIndex = 0;
	for (int i = myUnitIndex; i < myUnits.Size() - 1; ++i)
	{
		if (myUnits[i]->GetSubType() == CU::ToLower(aUnitType) && myUnits[i]->IsAlive() == false)
		{
			return myUnits[i];
		}
		myUnitIndex++;
	}
	return nullptr;
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

const CU::GrowingArray<Entity*>& SharedUnitManager::GetUnits()
{
	return myUnits;
}
