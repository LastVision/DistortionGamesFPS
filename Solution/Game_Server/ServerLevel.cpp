#include "stdafx.h"
#include "ServerLevel.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <NetworkAddPlayerMessage.h>
#include <PostMaster.h>
ServerLevel::ServerLevel()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
}

ServerLevel::~ServerLevel()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
}

void ServerLevel::onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		const physx::PxTriggerPair& cp = pairs[i];

		//if (pairs->triggerActor == myPlayer
		//{
		//
		//}
	}
}

void ServerLevel::Update(const float aDeltaTime)
{
	aDeltaTime;
}

void ServerLevel::ReceiveMessage(const NetworkAddPlayerMessage& aMessage)
{
	aMessage;
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newPlayer = EntityFactory::CreateEntity(eEntityType::UNIT, "player", nullptr, false, { 0.f, 0.f, 0.f });
	//newPlayer->AddToScene();
	newPlayer->Reset();
	myPlayers.Add(newPlayer);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}
