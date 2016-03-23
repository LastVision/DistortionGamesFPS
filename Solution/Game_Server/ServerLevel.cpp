#include "stdafx.h"
#include "ServerLevel.h"

#include <CollisionNote.h>
#include <DefendTouchMessage.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <GrenadeComponent.h>
#include <HealthComponent.h>
#include "MissionManager.h"
#include <NetMessageLevelLoaded.h>
#include <NetMessageSetActive.h>
#include <NetMessageHealthPack.h>
#include <NetMessageEntityState.h>
#include <NetMessageShootGrenade.h>
#include <NetworkComponent.h>
#include <PhysicsInterface.h>
#include <PhysicsComponent.h>
#include <BulletComponent.h>
#include "ServerNetworkManager.h"
#include <TriggerComponent.h>
#include <PollingStation.h>
#include <PostMaster.h>
#include <SetActiveMessage.h>
#include <RespawnMessage.h>
#include <RespawnTriggerMessage.h>

#include "ServerProjectileManager.h"

ServerLevel::ServerLevel()
	: myLoadedClients(16)
	, myAllClientsLoaded(false)
	, myNextLevel(-1)
	, myRespawnTriggers(16)
{
	Prism::PhysicsInterface::Create(std::bind(&ServerLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), true);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::LEVEL_LOADED, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENTITY_STATE, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::SHOOT_GRENADE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::SET_ACTIVE, this);
	ServerProjectileManager::Create();
	PostMaster::GetInstance()->Subscribe(eMessageType::RESPAWN_TRIGGER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::RESPAWN, this);

	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::HEALTH_PACK, this);
}

ServerLevel::~ServerLevel()
{
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif
	ServerProjectileManager::Destroy();
	PollingStation::Destroy();
	SAFE_DELETE(myMissionManager);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_LOADED, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENTITY_STATE, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SHOOT_GRENADE, this);

	PostMaster::GetInstance()->UnSubscribe(eMessageType::SET_ACTIVE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::RESPAWN_TRIGGER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::RESPAWN, this);

	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::HEALTH_PACK, this);
}

void ServerLevel::Init(const std::string& aMissionXMLPath)
{
	for (int i = 0; i < ServerNetworkManager::GetInstance()->GetClients().Size(); ++i)
	{
		const Connection& client = ServerNetworkManager::GetInstance()->GetClients()[i];

		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "playerserver", nullptr, false, { 0.f, 0.f, 0.f });
		newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
		myPlayers.Add(newPlayer);

		Entity* newRespawnTrigger = EntityFactory::CreateEntity(99983 + i, eEntityType::TRIGGER, "respawn", nullptr, false, { 0.f, -1.f, 0.f });
		newRespawnTrigger->GetComponent<TriggerComponent>()->SetRespawnValue(i + 1);
		myRespawnTriggers.Add(newRespawnTrigger);

		myMissionManager = new MissionManager(aMissionXMLPath);
	}
	ServerProjectileManager::GetInstance()->CreateBullets(nullptr);
	ServerProjectileManager::GetInstance()->CreateGrenades(nullptr);

}

void ServerLevel::Update(const float aDeltaTime)
{
	if (myAllClientsLoaded == true && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		__super::Update(aDeltaTime);
		myMissionManager->Update(aDeltaTime);
		ServerProjectileManager::GetInstance()->Update(aDeltaTime);
		//	PollingStation::GetInstance()->FindClosestEntityToEntity(*myPlayers[0]);

		Prism::PhysicsInterface::GetInstance()->EndFrame();
	}

	for each (Entity* trigger in myRespawnTriggers)
	{
		trigger->Update(aDeltaTime);
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

void ServerLevel::ReceiveNetworkMessage(const NetMessageHealthPack& aMessage, const sockaddr_in&)
{
	myPlayers[aMessage.mySenderID - 1]->GetComponent<HealthComponent>()->Heal(aMessage.myHealAmount);
}

void ServerLevel::ReceiveNetworkMessage(const NetMessageShootGrenade& aMessage, const sockaddr_in&)
{
	Entity* bullet = ServerProjectileManager::GetInstance()->RequestGrenade();
	CU::Matrix44<float> playerOrientation = myPlayers[aMessage.mySenderID - 1]->GetOrientation();

	bullet->Reset();
	CU::Vector3<float> pos = playerOrientation.GetPos();
	pos.y += 1.5f;
	bullet->GetComponent<PhysicsComponent>()->TeleportToPosition(pos);
	bullet->GetComponent<GrenadeComponent>()->Activate(aMessage.mySenderID);
	bullet->GetComponent<PhysicsComponent>()->AddForce(playerOrientation.GetForward(), float(aMessage.myForceStrength));

	//Skicka samma meddelande till clienten
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageShootGrenade>(NetMessageShootGrenade(aMessage.myForceStrength));
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

void ServerLevel::ReceiveMessage(const RespawnMessage &aMessage)
{
	int gid = aMessage.myGID - 1;

	myPlayers[gid]->GetComponent<HealthComponent>()->Heal(myPlayers[gid]->GetComponent<HealthComponent>()->GetMaxHealth());
	myPlayers[gid]->SetState(eEntityState::IDLE);
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(eEntityState::IDLE, gid + 1), gid);
	myRespawnTriggers[gid]->GetComponent<PhysicsComponent>()->RemoveFromScene();
	myPlayers[gid]->GetComponent<PhysicsComponent>()->Wake();
}

void ServerLevel::ReceiveMessage(const RespawnTriggerMessage& aMessage)
{
	myRespawnTriggers[aMessage.myGID - 1]->GetComponent<TriggerComponent>()->Activate();
	myRespawnTriggers[aMessage.myGID - 1]->GetComponent<PhysicsComponent>()->AddToScene();
	myRespawnTriggers[aMessage.myGID - 1]->GetComponent<PhysicsComponent>()->TeleportToPosition(myPlayers[aMessage.myGID - 1]->GetOrientation().GetPos());
}

void ServerLevel::HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered)
{
	TriggerComponent* firstTrigger = aFirstEntity.GetComponent<TriggerComponent>();
	if (aHasEntered == true)
	{
		if (aSecondEntity.GetType() == eEntityType::UNIT && aSecondEntity.GetSubType() == "playerserver")
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
			case eTriggerType::LEVEL_CHANGE:
				myNextLevel = firstTrigger->GetValue();
				break;
			//case eTriggerType::RESPAWN:
			//	myPlayers[firstTrigger->GetRespawnValue() - 1]->GetComponent<HealthComponent>()->Heal(myPlayers[firstTrigger->GetRespawnValue() - 1]->GetComponent<HealthComponent>()->GetMaxHealth());
			//	myPlayers[firstTrigger->GetRespawnValue() - 1]->SetState(eEntityState::IDLE);
			//	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(eEntityState::IDLE, firstTrigger->GetRespawnValue()), firstTrigger->GetRespawnValue() - 1);
			//	myRespawnTriggers[firstTrigger->GetRespawnValue() - 1]->GetComponent<PhysicsComponent>()->RemoveFromScene();
			//	myPlayers[firstTrigger->GetRespawnValue() - 1]->GetComponent<PhysicsComponent>()->Wake();
			//	break;
			}
			aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity, aHasEntered));
		}
		else if (aSecondEntity.GetType() == eEntityType::UNIT && aSecondEntity.GetSubType() != "playerserver")
		{
			if (myMissionManager->GetCurrentMissionType() == eMissionType::DEFEND)
			{
				PostMaster::GetInstance()->SendMessage<DefendTouchMessage>(DefendTouchMessage(true));
			}
		}
	}
	else
	{
		if (aSecondEntity.GetType() == eEntityType::UNIT && aSecondEntity.GetSubType() != "playerserver")
		{
			if (myMissionManager->GetCurrentMissionType() == eMissionType::DEFEND)
			{
				PostMaster::GetInstance()->SendMessage<DefendTouchMessage>(DefendTouchMessage(false));
			}
		}
	}
	aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity, aHasEntered));
}

bool ServerLevel::ChangeLevel(int& aNextLevel)
{
	if (myNextLevel != -1)
	{
		aNextLevel = myNextLevel;
		return true;
	}
	return false;
}

