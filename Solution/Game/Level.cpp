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

Level::Level()
	: myEntities(512)
	, myClients(16)
{
	Prism::PhysicsInterface::Create();
	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());
	//myTempPosition = { 835.f, 0.f, -1000.f };
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
		ClientNetworkManager::GetInstance()->ConnectToServer();
	}
	DEBUG_PRINT(ClientNetworkManager::GetInstance()->GetResponsTime());
	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetCamera());

	Prism::PhysicsInterface::GetInstance()->Update();
}

void Level::Render()
{
	myScene->Render();
	myPlayer->Render();

	/*for (int i = 0; i < myClients.Size(); i++)
	{
		Prism::DebugDrawer::GetInstance()->RenderBox(myClients[i].myPosition, eColorDebug::BLUE, 1.f);
	}*/


}