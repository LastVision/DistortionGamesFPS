#include "stdafx.h"
#include "ClientUnitManager.h"
#include <EntityFactory.h>
#include <PhysicsComponent.h>

ClientUnitManager::ClientUnitManager()
{
}


ClientUnitManager::~ClientUnitManager()
{
}

void ClientUnitManager::CreateUnits(Prism::Scene* aScene)
{
	SET_RUNTIME(false);
	CU::GrowingArray<std::string> types(8);
	std::fstream read;
	read.open("Data/Setting/SET_unitTypes.dtxt");

	for (unsigned int i = 0; i < types.GetCapacity(); ++i)
	{
		std::string toAdd;
		read >> toAdd;

		if (toAdd != "")
		{
			types.Add(toAdd);
		}
	}
	int index = 51000;
	for (unsigned int j = 0; j < types.Size(); ++j)
	{
		for (unsigned int i = 0; i < myUnits.GetCapacity(); ++i)
		{
			Entity* unit = EntityFactory::CreateEntity(index++, eEntityType::UNIT, CU::ToLower(types[j]), aScene, true, CU::Vector3<float>());
			unit->GetComponent<PhysicsComponent>()->RemoveFromScene();
			unit->Kill();
			myUnits.Add(unit);
		}
	}

}

ClientUnitManager* ClientUnitManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ClientUnitManager doesn't exist. Please create it before getting it!");
	return static_cast<ClientUnitManager*>(myInstance);
}

void ClientUnitManager::Create()
{
	DL_ASSERT_EXP(myInstance == nullptr, "ClientUnitManager already exists.");
	SET_RUNTIME(false);
	myInstance = new ClientUnitManager();
}

void ClientUnitManager::Destroy()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ClientUnitManager doesn't exist... Can't destroy something that doesn't exist!");
	SAFE_DELETE(myInstance);
}
