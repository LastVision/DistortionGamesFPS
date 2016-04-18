#include "stdafx.h"
#include <AnimationComponent.h>
#include <AudioInterface.h>
#include "ClientUnitManager.h"
#include <EntityFactory.h>
#include <PhysicsComponent.h>
#include <NetMessageActivateUnit.h>
#include "ClientNetworkManager.h"
#include "NetMessageEntityState.h"
#include <SoundComponent.h>
#include <PostMaster.h>
#include <EmitterMessage.h>
#include <Intersection.h>

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
	std::ifstream read;
	read.open("Data/Setting/SET_unitTypes.dtxt");

	for (int i = 0; i < types.GetCapacity(); ++i)
	{
		std::string toAdd;
		read >> toAdd;

		if (toAdd != "")
		{
			types.Add(toAdd);
		}
	}
	int index = 51000;
	for (int j = 0; j < types.Size(); ++j)
	{
		for (int i = 0; i < myUnits.GetCapacity() / types.Size(); ++i)
		{
			Entity* unit = EntityFactory::CreateEntity(index++, eEntityType::UNIT, CU::ToLower(types[j]), aScene, true
				, CU::Vector3<float>(0.f, -200.f, 0.f));

			unit->SetIsEnemy(true);
			unit->GetComponent<PhysicsComponent>()->RemoveFromScene();
			unit->Kill();
			unit->SetActive(false);
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
	PostMaster::GetInstance()->SendMessage(EmitterMessage("SpawnEnemy", { aMessage.myPosition.x
		, aMessage.myPosition.y + 1.5f
		, aMessage.myPosition.z }
	, CU::Vector3<float>(0.f, -1.f, 0.f)));

	Entity* toActivate = RequestUnit(aMessage.myGID);
	if (toActivate != nullptr)
	{
		ActivateUnit(toActivate, aMessage.myPosition);
		if (toActivate->GetComponent<SoundComponent>() != nullptr)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EnemySpawn"
				, toActivate->GetComponent<SoundComponent>()->GetAudioSFXID());
		}
	}
}

void ClientUnitManager::ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in&)
{
	if (myUnitsMap.find(aMessage.myGID) != myUnitsMap.end())
	{
		Entity* unit = myUnitsMap[aMessage.myGID];
		unit->SetState(static_cast<eEntityState>(aMessage.myEntityState));
		if (aMessage.myEntityState == static_cast<unsigned char>(eEntityState::DIE))
		{
			if (unit->GetComponent<SoundComponent>() != nullptr)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EnemyDie", unit->GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			//myUnitsMap[aMessage.myGID]->Kill();
			unit->GetComponent<PhysicsComponent>()->RemoveFromScene();
			unit->SetActive(false);
			unit->GetComponent<AnimationComponent>()->StopMuzzleFlash();
		}
	}
}

int ClientUnitManager::GetUnitsInPlayerRange(const CU::Vector3<float>& aPlayerPos) const
{
	int toReturn = 0;

	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		if (myActiveUnits[i]->GetState() != eEntityState::DIE && CU::Length2(myActiveUnits[i]->GetOrientation().GetPos() - aPlayerPos) < 900.f)
		{
			++toReturn;
		}
	}
	return toReturn;
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
