#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include "NetworkManager.h"
#include "NetworkMessageTypes.h"
#include "NetworkMessageTypes.h"

#include "DebugDrawer.h"

#include "EntityFactory.h"
#include "Entity.h"
#include "GameEnum.h"
#include <PhysicsInterface.h>

Level::Level()
	: myEntities(512)
{
	connected = false;
	Prism::PhysicsInterface::Create();
	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());

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
	NetworkManager::GetInstance()->Swap(true); 
	myPlayer->Update(aDeltaTime);
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_C) == true)
	{
		NetworkManager::GetInstance()->ConnectToServer();
		//NetworkManager::GetInstance()->ConnectToServer("172.22.53.245"); //LinusS IP
	}

	const CU::GrowingArray<Buffer>& messages = NetworkManager::GetInstance()->GetReceieveBuffer();
	for (Buffer buf : messages)
	{
		eNetMessageType type = NetworkManager::GetInstance()->ReadType(&buf.myData[0]);

		switch (type)
		{
		case eNetMessageType::ON_JOIN:
			connected = true;
			break;
		}
	}

	if (connected == true)
	{
		DEBUG_PRINT("Player 2 has joined!");
	}
	else
	{
		DEBUG_PRINT("Nobody is here!");
	}

	/*for (int i = 0; i < messages.Size(); ++i)
	{

	if (messages[i].myID == '\x2')
	{
	connected = true;
	messages.RemoveAll();
	}
	}*/
	Prism::PhysicsInterface::GetInstance()->Update();
}

void Level::Render()
{
	myScene->Render();
	myPlayer->Render();
}
