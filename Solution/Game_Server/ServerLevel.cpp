#include "stdafx.h"
#include "ServerLevel.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageLevelLoaded.h>
#include <NetworkComponent.h>
#include <NetMessageAddEnemy.h>
#include <PhysicsInterface.h>
#include "ServerNetworkManager.h"

ServerLevel::ServerLevel()
	: myLoadedClients(16)
{
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
}

ServerLevel::~ServerLevel()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
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
		for (Entity* e : myActiveEnemies)
		{
			ServerNetworkManager::GetInstance()->AddMessage(NetMessageAddEnemy(e->GetOrientation().GetPos(), e->GetGID()));
		}
	}
}
