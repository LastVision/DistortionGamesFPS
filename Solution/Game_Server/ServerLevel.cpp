#include "stdafx.h"
#include "ServerLevel.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageConnectMessage.h>
#include <NetworkComponent.h>
#include <PhysicsInterface.h>
#include "ServerNetworkManager.h"

ServerLevel::ServerLevel()
{
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
}

ServerLevel::~ServerLevel()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
}

void ServerLevel::Update(const float aDeltaTime)
{
	__super::Update(aDeltaTime);

	Prism::PhysicsInterface::GetInstance()->EndFrame();
}

void ServerLevel::ReceiveNetworkMessage(const NetMessageConnectMessage&, const sockaddr_in&)
{
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newPlayer = EntityFactory::CreateEntity(ServerNetworkManager::GetInstance()->GetLastJoinedID(), eEntityType::UNIT, "player", nullptr, false, { 0.f, 0.f, 0.f });
	newPlayer->Reset();
	newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
	myPlayers.Add(newPlayer);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);

	for (Entity* e : myActiveEnemies)
	{
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageAddEnemy(e->GetOrientation().GetPos(), e->GetGID()));
	}
}