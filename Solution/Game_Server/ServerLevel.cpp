#include "stdafx.h"
#include "ServerLevel.h"

#include <CollisionNote.h>
#include <DefendTouchMessage.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <HealthComponent.h>
#include "MissionManager.h"
#include <NetMessageRequestConnect.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageSetActive.h>
#include <NetMessageEntityState.h>
#include <NetworkComponent.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include <BulletComponent.h>
#include "ServerNetworkManager.h"
#include <TriggerComponent.h>
#include <PostMaster.h>
#include <SetActiveMessage.h>

#include <PollingStation.h>
#include "ServerProjectileManager.h"

ServerLevel::ServerLevel()
	: myLoadedClients(16)
	, myAllClientsLoaded(false)
{
	Prism::PhysicsInterface::Create(std::bind(&ServerLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), true);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::LEVEL_LOADED, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENTITY_STATE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::SET_ACTIVE, this);
	ServerProjectileManager::Create();
}

ServerLevel::~ServerLevel()
{
	ServerProjectileManager::Destroy();
	PollingStation::Destroy();
	SAFE_DELETE(myMissionManager);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_LOADED, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENTITY_STATE, this);

	PostMaster::GetInstance()->UnSubscribe(eMessageType::SET_ACTIVE, this);
}

void ServerLevel::Init(const std::string& aMissionXMLPath)
{
	for each (const Connection& client in ServerNetworkManager::GetInstance()->GetClients())
	{
		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "player", nullptr, false, { 0.f, 0.f, 0.f });
		newPlayer->Reset();
		newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
		myPlayers.Add(newPlayer);

		myMissionManager = new MissionManager(aMissionXMLPath);
	}
}

void ServerLevel::Update(const float aDeltaTime)
{
	if (myAllClientsLoaded == true && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		__super::Update(aDeltaTime);
		myMissionManager->Update(aDeltaTime);
		//	PollingStation::GetInstance()->FindClosestEntityToEntity(*myPlayers[0]);

		Prism::PhysicsInterface::GetInstance()->EndFrame();
	}
}

void ServerLevel::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered)
{
	Entity& first = aFirst->GetEntity();
	Entity& second = aSecond->GetEntity();

	switch (first.GetType())
	{
	case eEntityType::TRIGGER:
		HandleTrigger(first, second, aHasEntered);
		break;
	case eEntityType::EXPLOSION:
		if (aHasEntered == true)
		{
			SharedLevel::HandleExplosion(first, second);
		}
		break;
	case eEntityType::BULLET:
		if (second.GetComponent<HealthComponent>() != nullptr)
		{
			second.GetComponent<HealthComponent>()->TakeDamage(first.GetComponent<BulletComponent>()->GetDamage());
		}
		first.Kill();
		break;
	}
}

void ServerLevel::ReceiveNetworkMessage(const NetMessageRequestConnect&, const sockaddr_in&)
{
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newPlayer = EntityFactory::CreateEntity(ServerNetworkManager::GetInstance()->GetLastJoinedID(), eEntityType::UNIT, "player", nullptr, false, { 0.f, 0.f, 0.f });
	newPlayer->Reset();
	newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
	myPlayers.Add(newPlayer);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}

void ServerLevel::ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in&)
{
	myLoadedClients.Add(aMessage.mySenderID);

	if (ServerNetworkManager::GetInstance()->ListContainsAllClients(myLoadedClients) == true)
	{
		myAllClientsLoaded = true;

		//for (Entity* e : myActiveEnemies)
		//{
		//	ServerNetworkManager::GetInstance()->AddMessage(NetMessageAddEnemy(e->GetOrientation().GetPos(), e->GetGID()));
		//}
	}
}

void ServerLevel::ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in&)
{
	for each (const Connection& client in ServerNetworkManager::GetInstance()->GetClients())
	{
		ServerNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(aMessage, client.myID);
	}
}

void ServerLevel::ReceiveMessage(const SetActiveMessage& aMessage)
{
	if (aMessage.myShouldActivate == true)
	{
		myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->AddToScene();
	}
	else
	{
		myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
}

void ServerLevel::HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered)
{
	TriggerComponent* firstTrigger = aFirstEntity.GetComponent<TriggerComponent>();
	if (aHasEntered == true)
	{
		if (aSecondEntity.GetType() == eEntityType::UNIT && aSecondEntity.GetSubType() == "player")
		{
			switch (firstTrigger->GetTriggerType())
			{
			case eTriggerType::UNLOCK:
				SharedNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(false, true, firstTrigger->GetValue()));
				myActiveEntitiesMap[firstTrigger->GetValue()]->GetComponent<PhysicsComponent>()->RemoveFromScene();
				// do "open" animation
				break;
			case eTriggerType::LOCK:
				SharedNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(true, true, firstTrigger->GetValue()));
				myActiveEntitiesMap[firstTrigger->GetValue()]->GetComponent<PhysicsComponent>()->AddToScene();
				// do "close" animation
				break;
			case eTriggerType::MISSION:
				myMissionManager->SetMission(firstTrigger->GetValue());
				break;
			}
			aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity));
			aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity));
		}
		else if (aSecondEntity.GetType() == eEntityType::UNIT && aSecondEntity.GetSubType() != "player")
		{
			if (myMissionManager->GetCurrentMissionType() == eMissionType::DEFEND)
			{
				PostMaster::GetInstance()->SendMessage<DefendTouchMessage>(DefendTouchMessage(true));
			}
		}
	}
	else
	{
		if (aSecondEntity.GetType() == eEntityType::UNIT && aSecondEntity.GetSubType() != "player")
		{
			if (myMissionManager->GetCurrentMissionType() == eMissionType::DEFEND)
			{
				PostMaster::GetInstance()->SendMessage<DefendTouchMessage>(DefendTouchMessage(false));
			}
		}
	}
	


}

