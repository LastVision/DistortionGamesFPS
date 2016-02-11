#include "stdafx.h"

#include <EntityEnumConverter.h>
#include <EntityFactory.h>
#include <MathHelper.h>
#include "NeutralDirector.h"
#include <PollingStation.h>
#include <PostMaster.h>

#include <SpawnUnitMessage.h>
#include <TimeMultiplierMessage.h>
#include <Terrain.h>


NeutralDirector::NeutralDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene)
	: Director(eOwnerType::NEUTRAL, aTerrain)
{

	myUnits.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL, eEntityType::UNIT, "non_attack_tutorial"
		, Prism::eOctreeType::DYNAMIC, aScene, { 20.f, 0.f, 40.f }, aTerrain));

	for (int i = 0; i < 64; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL, eEntityType::UNIT, "grunt", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL, eEntityType::UNIT, "ranger", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL, eEntityType::UNIT, "tank", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}


	/*for (int i = 0; i < 2; ++i)
	{
		myActiveUnits.Add(myUnits[i]);
		myActiveUnits[i]->Spawn({ 140.f, 0.f, 130.f });
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits[i]);
	}*/

	PostMaster::GetInstance()->Subscribe(eMessageType::TIME_MULTIPLIER, this);
}


NeutralDirector::~NeutralDirector()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TIME_MULTIPLIER, this);

}

void NeutralDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (aMessage.myOwnerType == static_cast<int>(eOwnerType::NEUTRAL) && myActiveUnits.Size() < 64)
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetUnitType() == static_cast<eUnitType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
			{
				if (IsAlreadyActive(myUnits[i]) == true)
				{
					continue;
				}
				myUnits[i]->Spawn(aMessage.mySpawnPoint, aMessage.myRallyPoint);
				myActiveUnits.Add(myUnits[i]);
				break;
			}
		}

		PollingStation::GetInstance()->RegisterEntity(myActiveUnits.GetLast());
	}
}

void NeutralDirector::ReceiveMessage(const TimeMultiplierMessage& aMessage)
{
	if (aMessage.myOwner == eOwnerType::ENEMY)
	{
		myTimeMultiplier = aMessage.myMultiplier;
	}
}
