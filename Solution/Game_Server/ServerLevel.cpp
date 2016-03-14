#include "stdafx.h"
#include "ServerLevel.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <NetworkAddPlayerMessage.h>
#include <NetworkAddEnemyMessage.h>
#include <PostMaster.h>
#include <NetworkComponent.h>
#include <PhysicsInterface.h>

ServerLevel::ServerLevel()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
}

ServerLevel::~ServerLevel()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
}

void ServerLevel::Update(const float aDeltaTime)
{
	__super::Update(aDeltaTime);

	Prism::PhysicsInterface::GetInstance()->EndFrame();
}

void ServerLevel::ReceiveMessage(const NetworkAddPlayerMessage& aMessage)
{
	aMessage;
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
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
			PostMaster::GetInstance()->SendMessage(NetworkAddEnemyMessage({ 0.f, 0.f, 0.f }, e->GetGID(), aMessage.myAddress));
		}
	}
}
