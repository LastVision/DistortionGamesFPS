#include "stdafx.h"
#include "ServerLevel.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <PostMasterNetAddPlayerMessage.h>
#include <PostMasterNetAddEnemyMessage.h>
#include <PostMasterNetLevelLoadedMessage.h>
#include <PostMaster.h>
#include <NetworkComponent.h>
#include <NetMessageAddEnemy.h>
#include <PhysicsInterface.h>
#include "ServerNetworkManager.h"

ServerLevel::ServerLevel()
	: myLoadedClients(16)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_LEVEL_LOADED, this);
}

ServerLevel::~ServerLevel()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_LEVEL_LOADED, this);
}

void ServerLevel::Init()
{
	for each (const Connection& client in ServerNetworkManager::GetInstance()->GetClients())
	{
		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "player", nullptr, false, { 0.f, 0.f, 0.f });
		newPlayer->Reset();
		newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
		myPlayers.Add(newPlayer);
	}
}

void ServerLevel::Update(const float aDeltaTime)
{
	__super::Update(aDeltaTime);

	Prism::PhysicsInterface::GetInstance()->EndFrame();
}

void ServerLevel::ReceiveMessage(const PostMasterNetAddPlayerMessage& aMessage)
{
	aMessage;
	/*bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newPlayer = EntityFactory::CreateEntity(aMessage.myGID, eEntityType::UNIT, "player", nullptr, false, { 0.f, 0.f, 0.f });
	newPlayer->Reset();
	newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
	myPlayers.Add(newPlayer);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);

	for (Entity* e : myActiveEnemies)
	{
		if (e->GetComponent<NetworkComponent>() != nullptr)
		{
			PostMaster::GetInstance()->SendMessage(PostMasterNetAddEnemyMessage({ 0.f, 0.f, 0.f }, e->GetGID(), aMessage.myAddress));
		}
	}*/
}

void ServerLevel::ReceiveMessage(const PostMasterNetLevelLoadedMessage& aMessage)
{
	myLoadedClients.Add(aMessage.mySender);

	if (ServerNetworkManager::GetInstance()->ListContainsAllClients(myLoadedClients) == true)
	{
		for (Entity* e : myActiveEnemies)
		{
			if (e->GetComponent<NetworkComponent>() != nullptr)
			{
				ServerNetworkManager::GetInstance()->AddMessage(NetMessageAddEnemy({ 0.f, 0.f, 0.f }, e->GetGID()));
			}
		}
	}
}
