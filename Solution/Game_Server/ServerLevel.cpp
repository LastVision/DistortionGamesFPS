#include "stdafx.h"
#include "ServerLevel.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <NetMessageAddEnemy.h>
#include <NetMessageConnectMessage.h>
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
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageAddEnemy(e->GetOrientation().GetPos(), e->GetGID()));
	}
}
