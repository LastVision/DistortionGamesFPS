#include "stdafx.h"
#include "ClientUnitManager.h"
#include <EntityFactory.h>
#include <PhysicsComponent.h>
#include <NetMessageActivateUnit.h>
#include "ClientNetworkManager.h"
#include "NetMessageEntityState.h"

ClientUnitManager::ClientUnitManager()
{
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ACTIVATE_UNIT, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENTITY_STATE, this);
}


ClientUnitManager::~ClientUnitManager()
{
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ACTIVATE_UNIT, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENTITY_STATE, this);

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
			Entity* unit = EntityFactory::CreateEntity(index++, eEntityType::UNIT, CU::ToLower(types[j]), aScene, true, CU::Vector3<float>(0.f, -200.f, 0.f));
			unit->GetComponent<PhysicsComponent>()->RemoveFromScene();
			unit->Kill();
			myUnits.Add(unit);
		}
	}

	AddToMap();

}

ClientUnitManager* ClientUnitManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ClientUnitManager doesn't exist. Please create it before getting it!");
	return static_cast<ClientUnitManager*>(myInstance);
}

void ClientUnitManager::ReceiveNetworkMessage(const NetMessageActivateUnit& aMessage, const sockaddr_in&)
{
	Entity* toActivate = RequestUnit(aMessage.myGID);
	if (toActivate != nullptr)
	{
		ActivateUnit(toActivate, aMessage.myPosition);
	}
}

void ClientUnitManager::ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in&)
{
	if (myUnitsMap.find(aMessage.myGID) != myUnitsMap.end())
	{
		myUnitsMap[aMessage.myGID]->SetState(static_cast<eEntityState>(aMessage.myEntityState));
		if (aMessage.myEntityState == static_cast<unsigned char>(eEntityState::DIE))
		{
			myUnitsMap[aMessage.myGID]->Kill();
		}
	}
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
