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

#include "EntityFactory.h"
#include "Entity.h"
#include <EntityData.h>
#include "GameEnum.h"
#include <PhysicsInterface.h>

#include <NetMessageOnJoin.h>
#include <NetMessageConnectMessage.h>
#include <NetMessageOnHit.h>

#include <NetworkAddPlayerMessage.h>
#include <NetworkAddEnemyMessage.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <PointLight.h>

#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include <PostMaster.h>

#include "EmitterManager.h"
#include <EmitterMessage.h>

#include <NetworkComponent.h>

ClientLevel::ClientLevel()
	: myInstanceOrientations(16)
	, myInstances(16)
	, myPointLights(64)
{
	//Prism::PhysicsInterface::Destroy();
	//Prism::PhysicsInterface::GetInstance()->RayCast({ 0, 0, 0 }, { 0, 1, 0 }, 10.f);
	//Prism::PhysicsInterface::GetInstance()->Update();
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_ENEMY, this);

	myScene = new Prism::Scene();
	EntityData data;
	data.myInputData.myExistsInEntity = true;
	//myPlayer = new Entity(data, myScene, true, CU::Vector3<float>(), CU::Vector3<float>(), CU::Vector3<float>(1.f, 1.f, 1.f));
	myPlayer = EntityFactory::GetInstance()->CreateEntity(eEntityType::UNIT, "player", myScene, true, CU::Vector3<float>());
	myScene->SetCamera(*myPlayer->GetComponent<InputComponent>()->GetCamera());

	//myTempPosition = { 835.f, 0.f, -1000.f };

	myDeferredRenderer = new Prism::DeferredRenderer();


	myEmitterManager = new EmitterManager(*myPlayer->GetComponent<InputComponent>()->GetCamera());
	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlaySecondLayer", 0);
}

ClientLevel::~ClientLevel()
{
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif

	SAFE_DELETE(myEmitterManager);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_ENEMY, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSecondLayer", 0);
}

void ClientLevel::Update(const float aDeltaTime)
{
	SharedLevel::Update(aDeltaTime);
	myPlayer->Update(aDeltaTime);
	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_T))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(eNetMessageType::PLAYER_ON_HIT, 5.f, 17));
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_J))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(eNetMessageType::PLAYER_ON_HIT, 5.f, 2));
	}


	unsigned short ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = static_cast<float>(ClientNetworkManager::GetInstance()->GetDataSent());

	DEBUG_PRINT(ms);
	DEBUG_PRINT(kbs);
	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetComponent<InputComponent>()->GetCamera());

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
	//myScene->Render();
	//myDeferredRenderer->Render(myScene);
	myPlayer->GetComponent<InputComponent>()->Render();
	myEmitterManager->RenderEmitters();
}

void ClientLevel::ReceiveMessage(const NetworkAddPlayerMessage& aMessage)
{
	aMessage;
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newPlayer = EntityFactory::CreateEntity(eEntityType::UNIT, "player", myScene, true, { 0.f, 0.f, 0.f });
	newPlayer->GetComponent<NetworkComponent>()->SetNetworkID(aMessage.myNetworkID);
	newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
	newPlayer->AddToScene();
	newPlayer->Reset();
	myPlayers.Add(newPlayer);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}

void ClientLevel::ReceiveMessage(const NetworkAddEnemyMessage& aMessage)
{
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newEnemy = EntityFactory::CreateEntity(eEntityType::UNIT, "gundroid", myScene, true, aMessage.myPosition);

	newEnemy->AddToScene();
	newEnemy->Reset();

	//PostMaster::GetInstance()->SendMessage(EmitterMessage("Example", aMessage.myPosition));
	newEnemy->GetComponent<NetworkComponent>()->SetNetworkID(aMessage.myNetworkID);

	myEnemies.Add(newEnemy);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}

void ClientLevel::AddLight(Prism::PointLight* aLight)
{
	myPointLights.Add(aLight);
	myScene->AddLight(aLight);
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