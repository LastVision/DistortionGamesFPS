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
		//aUnit->AddToScene();
		aUnit->SetDelayedAddToScene();
		aUnit->GetComponent<PhysicsComponent>()->AddToScene();
	}
	
	myActiveUnits.Add(aUnit);
}

Entity* SharedUnitManager::RequestUnit(const std::string& aUnitType)
{
	if (myActiveUnits.Size() < myActiveUnits.GetCapacity())
	{
		if (myUnits.Size() == 0)
		{
			MessageBox(GetActiveWindow(), "Failed to load enemies", "ERROR", MB_ICONERROR);
			return nullptr;
		}

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

Entity* SharedUnitManager::GetUnitToHit(const CU::Matrix44<float>& aPlayerOrientation) const
{
	float closest2 = FLT_MAX;
	Entity* toReturn = nullptr;

	float dot = 0.f;

	CU::Vector3<float> playerForward = aPlayerOrientation.GetForward();
	playerForward.y = 0.f;

	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		CU::Vector3<float> toEnemy = myActiveUnits[i]->GetOrientation().GetPos() - aPlayerOrientation.GetPos();
		float dist2 = CU::Length2(toEnemy);
		toEnemy.y = 0.2f;
		if (myActiveUnits[i]->GetState() != eEntityState::DIE && myActiveUnits[i]->IsAlive() == true && dist2 < closest2
			&& dist2 < 30.f*30.f)
		{
			//Try -GetForward() or -Z in GetForward()
			
			float thisDot = -CU::Dot(playerForward, CU::GetNormalized(toEnemy));

			if (thisDot > 0.9f)
			{
				dot = thisDot;
				closest2 = dist2;
				toReturn = myActiveUnits[i];
			}
		}
	}

	if (toReturn != nullptr)
	{
		OutputDebugString(CU::Concatenate("%f - HIT\n", dot).c_str());
	}
	else
	{
		OutputDebugString(CU::Concatenate("%f - NO HIT\n", dot).c_str());

	}

	//return nullptr;
	return toReturn;
}
