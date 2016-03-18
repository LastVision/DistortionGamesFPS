#include "stdafx.h"
#include "ServerLevel.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageLevelLoaded.h>
#include <NetworkComponent.h>
#include <PhysicsInterface.h>
#include "ServerNetworkManager.h"

ServerLevel::ServerLevel()
	: myLoadedClients(16)
	, myAllClientsLoaded(false)
{
	Prism::PhysicsInterface::Create(std::bind(&SharedLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2), true);

	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::LEVEL_LOADED, this);
}

ServerLevel::~ServerLevel()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_LOADED, this);
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
	if (myAllClientsLoaded == true && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		__super::Update(aDeltaTime);

		Prism::PhysicsInterface::GetInstance()->EndFrame();
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
