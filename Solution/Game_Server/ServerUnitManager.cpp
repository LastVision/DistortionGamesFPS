#include "stdafx.h"
#include "ServerUnitManager.h"
#include <GameEnum.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <fstream>
#include <CommonHelper.h>
ServerUnitManager::ServerUnitManager()
{
}


ServerUnitManager::~ServerUnitManager()
{
}

void ServerUnitManager::CreateUnits(Prism::Scene* aScene)
{
	SET_RUNTIME(false);
	CU::GrowingArray<std::string> types(8);
	std::fstream read;
	read.open("Data/Setting/SET_unitTypes.dtxt");

	for (int i = 0; i < types.GetCapacity(); ++i)
	{
		std::string toAdd;
		read >> toAdd;

		if (toAdd != "")
		{
			toAdd += "Server";
			types.Add(toAdd);
		}
	}
	int index = 51000;
	CU::Vector3<float> pos;
	for (int j = 0; j < types.Size(); ++j)
	{
		for (int i = 0; i < myUnits.GetCapacity(); ++i)
		{
			Entity* unit = EntityFactory::CreateEntity(index++, eEntityType::UNIT, CU::ToLower(types[j]), aScene, false, { pos.x, -pos.y + 5 * (j* i), pos.z });
			unit->Kill();
			myUnits.Add(unit);
		}
	}
}

ServerUnitManager* ServerUnitManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ServerUnitManager doesn't exist. Please create it before getting it!");
	return static_cast<ServerUnitManager*>(myInstance);
}

void ServerUnitManager::Create()
{
	DL_ASSERT_EXP(myInstance == nullptr, "ServerUnitManager already exists.");
	SET_RUNTIME(false);
	myInstance = new ServerUnitManager();
}

void ServerUnitManager::Destroy()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ServerUnitManager doesn't exist... Can't destroy something that doesn't exist!");
	SAFE_DELETE(myInstance);
}
