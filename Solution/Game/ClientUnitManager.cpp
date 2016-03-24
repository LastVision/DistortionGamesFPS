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
	CU::Vector3<float> pos;
	for (unsigned int j = 0; j < types.Size(); ++j)
	{
		for (unsigned int i = 0; i < myUnits.GetCapacity(); ++i)
		{
			pos = { pos.x, -pos.y + 5 * (j* i), pos.z };
			Entity* unit = EntityFactory::CreateEntity(index++, eEntityType::UNIT, CU::ToLower(types[j]), aScene, true, pos);
			unit->GetComponent<PhysicsComponent>()->RemoveFromScene();
			unit->GetComponent<PhysicsComponent>()->TeleportToPosition(pos);
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
