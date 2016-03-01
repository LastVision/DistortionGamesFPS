#include "stdafx.h"
#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include "NetworkMessageTypes.h"
#include "NetworkMessageTypes.h"
#include "NetMessagePosition.h"
#include "DebugDrawer.h"

#include "EntityFactory.h"
#include "Entity.h"
#include "GameEnum.h"
#include <NetMessageConnectMessage.h>
#include <PhysicsInterface.h>
#include "NetMessageOnJoin.h"

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <PointLight.h>

Level::Level()
	: myEntities(512)
	, myClients(16)
{
	Prism::PhysicsInterface::Create();
	//Prism::PhysicsInterface::Destroy();
	//Prism::PhysicsInterface::GetInstance()->RayCast({ 0, 0, 0 }, { 0, 1, 0 }, 10.f);
	//Prism::PhysicsInterface::GetInstance()->Update();

	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());

	Prism::PointLight* light = new Prism::PointLight();
	light->SetPosition({ 0.f, 1.f, 0.f, 0.f });
	light->SetColor({ 0.f, 1.f, 0.f, 1.f });
	light->SetRange(5.f);
	myScene->AddLight(light);

	Prism::PointLight* light2 = new Prism::PointLight();
	light2->SetPosition({ -1.f, 1.f, 1.f, 0.f });
	light2->SetColor({ 1.f, 0.f, 0.f, 1.f });
	light2->SetRange(5.f);
	myScene->AddLight(light2);

	Prism::PointLight* light3 = new Prism::PointLight();
	light3->SetPosition({ 1.f, 1.f, 1.f, 0.f });
	light3->SetColor({ 0.f, 0.f, 1.f, 1.f });
	light3->SetRange(5.f);
	myScene->AddLight(light3);
	//myTempPosition = { 835.f, 0.f, -1000.f };

	myDeferredRenderer = new Prism::DeferredRenderer();
}

Level::~Level()
{
	myEntities.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	Prism::PhysicsInterface::Destroy();
}

void Level::AddEntity(Entity* aEntity)
{
	myEntities.Add(aEntity);
}

void Level::Update(const float aDeltaTime)
{
	ClientNetworkManager::GetInstance()->Update(aDeltaTime);
	myPlayer->Update(aDeltaTime);

	for (int i = 0; i < myEntities.Size(); i++)
	{
		myEntities[i]->Update(aDeltaTime);
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_C) == true)
	{
		ClientNetworkManager::GetInstance()->ConnectToServer("81.170.227.192");
	}
	unsigned short ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = ClientNetworkManager::GetInstance()->GetDataSent();

	DEBUG_PRINT(ms);
	DEBUG_PRINT(kbs);
	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetCamera());

	Prism::PhysicsInterface::GetInstance()->Update();
}

void Level::Render()
{
	//myDeferredRenderer->Render(myScene);
	myScene->Render();
	myPlayer->Render();

	for (int i = 0; i < ClientNetworkManager::GetInstance()->GetClients().Size(); i++)
	{
		Prism::DebugDrawer::GetInstance()->RenderBox(ClientNetworkManager::GetInstance()->GetClients()[i].myPosition, eColorDebug::BLUE, 1.f);
	}


}