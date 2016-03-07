#include "stdafx.h"
#include <Instance.h>
#include "ClientLevel.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include "NetworkMessageTypes.h"

#include "DebugDrawer.h"

#include "EntityFactory.h"
#include "Entity.h"
#include "GameEnum.h"
#include <PhysicsInterface.h>

#include <NetMessageOnJoin.h>
#include <NetMessageConnectMessage.h>

#include <NetworkAddPlayerMessage.h>
#include <NetworkAddEnemyMessage.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <PointLight.h>

#include <PhysEntity.h>
#include <PostMaster.h>

ClientLevel::ClientLevel()
	: myInstanceOrientations(16)
	, myInstances(16)
	, myPointLights(64)
{
	Prism::PhysicsInterface::Create();
	//Prism::PhysicsInterface::Destroy();
	//Prism::PhysicsInterface::GetInstance()->RayCast({ 0, 0, 0 }, { 0, 1, 0 }, 10.f);
	//Prism::PhysicsInterface::GetInstance()->Update();
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::NETWORK_ADD_ENEMY, this);

	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());

	/*Prism::PointLight* light = new Prism::PointLight();
	light->SetPosition({ 1.f, 0.2f, 0.f });
	light->SetColor({ 0.f, 1.f, 0.f, 1.f });
	light->SetRange(5.f);
	myScene->AddLight(light);
	Prism::PointLight* light4 = new Prism::PointLight();
	light4->SetPosition({ 1.f, 0.2f, -0.5f });
	light4->SetColor({ 1.f, 0.f, 0.f, 1.f });
	light4->SetRange(5.f);
	myScene->AddLight(light4);
	Prism::PointLight* light5 = new Prism::PointLight();
	light5->SetPosition({ 1.f, 0.2f, -1.f });
	light5->SetColor({ 0.f, 0.f, 1.f, 1.f });
	light5->SetRange(5.f);
	myScene->AddLight(light5);

	Prism::PointLight* light2 = new Prism::PointLight();
	light2->SetPosition({ 1.f, 1.f, -10.f });
	light2->SetColor({ 0.f, 0.f, 1.f, 1.f });
	light2->SetRange(10.f);
	myScene->AddLight(light2);

	Prism::PointLight* light3 = new Prism::PointLight();
	light3->SetPosition({ 1.f, 1.f, -56.f });
	light3->SetColor({ 1.f, 0.f, 1.f, 1.f });
	light3->SetRange(10.f);
	myScene->AddLight(light3);*/

	//myTempPosition = { 835.f, 0.f, -1000.f };

	myDeferredRenderer = new Prism::DeferredRenderer();

	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);
}

ClientLevel::~ClientLevel()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_PLAYER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::NETWORK_ADD_ENEMY, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);
	Prism::PhysicsInterface::Destroy();
}

void ClientLevel::Update(const float aDeltaTime)
{
	SharedLevel::Update(aDeltaTime);
	ClientNetworkManager::GetInstance()->Update(aDeltaTime);
	myPlayer->Update(aDeltaTime);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_C) == true)
	{
		ClientNetworkManager::GetInstance()->ConnectToServer("81.170.227.192");
	}
	unsigned short ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = static_cast<float>(ClientNetworkManager::GetInstance()->GetDataSent());

	DEBUG_PRINT(ms);
	DEBUG_PRINT(kbs);
	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetCamera());

	for (int i = 0; i < myPlayers.Size(); ++i)
	{
		CU::Vector3f position = ClientNetworkManager::GetInstance()->GetClients()[i].myPosition;
		myPlayers[i]->SetPosition(position);
	}


	Prism::PhysicsInterface::GetInstance()->Update();
}

void ClientLevel::Render()
{
	myDeferredRenderer->Render(myScene);
	//myScene->Render();
	myPlayer->Render();
}

void ClientLevel::ReceiveMessage(const NetworkAddPlayerMessage& aMessage)
{
	aMessage;
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newPlayer = EntityFactory::CreateEntity(eEntityType::UNIT, "player", myScene, true, { 0.f, 0.f, 0.f });
	newPlayer->AddToScene();
	newPlayer->Reset();
	myPlayers.Add(newPlayer);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}

void ClientLevel::ReceiveMessage(const NetworkAddEnemyMessage& aMessage)
{
	bool isRunTime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	Entity* newEnemy = EntityFactory::CreateEntity(eEntityType::UNIT, "grunt", myScene, true, aMessage.myPosition);
	newEnemy->AddToScene();
	newEnemy->Reset();
	myEntities.Add(newEnemy);
	Prism::MemoryTracker::GetInstance()->SetRunTime(isRunTime);
}

void ClientLevel::AddLight(Prism::PointLight* aLight)
{
	myPointLights.Add(aLight);
	myScene->AddLight(aLight);
}