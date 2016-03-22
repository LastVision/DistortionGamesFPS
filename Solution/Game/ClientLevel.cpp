#include "stdafx.h"
#include <Instance.h>
#include <AudioInterface.h>

#include "ClientLevel.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include <InputComponent.h>
#include "NetworkMessageTypes.h"

#include "DebugDrawer.h"

#include <CollisionNote.h>
#include "EntityFactory.h"
#include "Entity.h"
#include <EntityData.h>
#include <FirstPersonRenderComponent.h>
#include "GameEnum.h"
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>

#include <NetMessageConnectReply.h>
#include <NetMessageDisconnect.h>
#include <NetMessageEntityState.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageOnDeath.h>
#include <NetMessageOnHit.h>
#include <NetMessageOnJoin.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageSetActive.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <PointLight.h>

#include <PhysicsInterface.h>
#include <PostMaster.h>
#include <TriggerComponent.h>
#include <UpgradeComponent.h>
#include <UpgradeNote.h>
#include "EmitterManager.h"
#include <EmitterMessage.h>

#include <NetworkComponent.h>

ClientLevel::ClientLevel()
	: myInstanceOrientations(16)
	, myInstances(16)
	, myPointLights(64)
	, myInitDone(false)
{
	Prism::PhysicsInterface::Create(std::bind(&ClientLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), false);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_REMOVE_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ON_DEATH, this);

	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DEATH, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_ACTIVE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENTITY_STATE, this);

	myScene = new Prism::Scene();
}

ClientLevel::~ClientLevel()
{
	if (ClientNetworkManager::GetInstance()->GetGID() > 0)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
	}
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif

	SAFE_DELETE(myEmitterManager);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_REMOVE_PLAYER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_ENEMY, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ON_DEATH, this);

	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DEATH, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_ACTIVE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENTITY_STATE, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSecondLayer", 0);
}

void ClientLevel::Init(const std::string&)
{
	CreatePlayers();

	Prism::ModelLoader::GetInstance()->Pause();
	myDeferredRenderer = new Prism::DeferredRenderer();

	myEmitterManager = new EmitterManager(*myPlayer->GetComponent<InputComponent>()->GetCamera());
	Prism::ModelLoader::GetInstance()->UnPause();
	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlaySecondLayer", 0);
}

void ClientLevel::Update(const float aDeltaTime)
{
	if (myInitDone == false && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		myInitDone = true;
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelLoaded());
	}

	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_U))
	//{
	//	myActiveEnemies.GetLast()->SetState(eEntityState::WALK);
	//}
	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_Y))
	//{
	//	myActiveEnemies.GetLast()->SetState(eEntityState::ATTACK);
	//}

	SharedLevel::Update(aDeltaTime);
	myPlayer->GetComponent<FirstPersonRenderComponent>()->UpdateCoOpPositions(myPlayers);
	myPlayer->Update(aDeltaTime);
	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_I))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
	}

	unsigned short ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = static_cast<float>(ClientNetworkManager::GetInstance()->GetDataSent());

	DEBUG_PRINT(ms);
	DEBUG_PRINT(kbs);

	//for (int i = 0; i < myPlayers.Size(); ++i)
	//{
	//	CU::Vector3f position = ClientNetworkManager::GetInstance()->GetClients()[i].myPosition;
	//	myPlayers[i]->SetPosition(position);
	//}

	DebugMusic();

	Prism::PhysicsInterface::GetInstance()->EndFrame();

	ClientNetworkManager::GetInstance()->Update(aDeltaTime);

}

void ClientLevel::Render()
{
	myDeferredRenderer->Render(myScene);
	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetComponent<InputComponent>()->GetCamera());
	//myScene->Render();
	//myDeferredRenderer->Render(myScene);
	myEmitterManager->RenderEmitters();
	myPlayer->GetComponent<FirstPersonRenderComponent>()->Render();
	//myPlayer->GetComponent<ShootingComponent>()->Render();
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in&)
{
	DL_ASSERT_EXP(aMessage.myGID != 0, "Can't kill server (id 0).");

	for (Entity* e : myActiveEnemies)
	{
		if (e->GetGID() == aMessage.myGID)
		{
			e->Kill();
		}
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in&)
{
	bool useEntityMap = true;

	if (myActiveEntitiesMap.find(aMessage.myGID) == myActiveEntitiesMap.end())
	{
		useEntityMap = false;
		//DL_ASSERT("GID NOT FOUND IN CLIENT LEVEL!");
	}
	else if (myActiveUnitsMap.find(aMessage.myGID) == myActiveUnitsMap.end())
	{
		int triggerWillNotDoAnythingOnClient = 0;
		return;
	}

	if (aMessage.myShouldActivate == true)
	{
		if (useEntityMap == true)
		{
			myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->AddToScene();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveEntitiesMap[aMessage.myGID]->AddToScene();
			}
		}
		else
		{
			myActiveUnitsMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->Wake();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveUnitsMap[aMessage.myGID]->AddToScene();
			}
		}
	}
	else
	{
		if (useEntityMap == true)
		{
			myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->RemoveFromScene();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveEntitiesMap[aMessage.myGID]->RemoveFromScene();
			}
		}
		else
		{
			myActiveUnitsMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->Sleep();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveUnitsMap[aMessage.myGID]->RemoveFromScene();
			}
		}
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in&)
{
	if (aMessage.myGID == myPlayer->GetGID())
	{
		return;
	}

	if (myActiveUnitsMap.find(aMessage.myGID) == myActiveUnitsMap.end())
	{
		DL_ASSERT("ENTITY GID NOT FOUND IN CLIENT LEVEL!");
	}

	myActiveUnitsMap[aMessage.myGID]->SetState(static_cast<eEntityState>(aMessage.myEntityState));
}

void ClientLevel::AddLight(Prism::PointLight* aLight)
{
	myPointLights.Add(aLight);
	myScene->AddLight(aLight);
}

void ClientLevel::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered)
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
			//HandleExplosion(first, second);
		}
		break;
	}
}

void ClientLevel::DebugMusic()
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInBackground", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_5) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutBackground", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_6) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInFirstLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_7) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutFirstLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_8) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInSecondLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_9) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutSecondLayer", 0);
	}
}

void ClientLevel::HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered)
{
	TriggerComponent* firstTrigger = aFirstEntity.GetComponent<TriggerComponent>();
	if (firstTrigger->GetTriggerType() == eTriggerType::UPGRADE && firstTrigger->GetTriggerType() == eTriggerType::HEALTH_PACK) 
	{
		if (aSecondEntity.GetType() == eEntityType::PLAYER)
		{
			aSecondEntity.SendNote<UpgradeNote>(aFirstEntity.GetComponent<UpgradeComponent>()->GetData());
		}
	}
	aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity));
	aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity));
}

void ClientLevel::CreatePlayers()
{
	myPlayer = EntityFactory::GetInstance()->CreateEntity(ClientNetworkManager::GetInstance()->GetGID(), eEntityType::UNIT, "localplayer", myScene, true, CU::Vector3<float>());

	myScene->SetCamera(*myPlayer->GetComponent<InputComponent>()->GetCamera());

	for each (const OtherClients& client in ClientNetworkManager::GetInstance()->GetClients())
	{
		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "player", myScene, true, { 0.f, 0.f, 0.f });
		newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
		newPlayer->AddToScene();
		newPlayer->Reset();
		myPlayers.Add(newPlayer);
		myActiveUnitsMap[newPlayer->GetGID()] = newPlayer;
	}
}